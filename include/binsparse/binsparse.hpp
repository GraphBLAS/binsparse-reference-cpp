#pragma once

#include "hdf5_tools.hpp"
#include "type_info.hpp"
#include <binsparse/containers/matrices.hpp>
#include <binsparse/detail.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <type_traits>

#include <binsparse/c_bindings/allocator_wrapper.hpp>
#include <binsparse/matrix_market/matrix_market.hpp>

namespace binsparse {

inline constexpr double version = 0.1;

// Dense Format

template <typename T, typename I, typename Order>
void write_dense_matrix(std::string fname, dense_matrix<T, I, Order> m,
                        nlohmann::json user_keys = {}) {
  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  std::span<T> values(m.values, m.m * m.n);

  hdf5_tools::write_dataset(f, "values", values);

  using json = nlohmann::json;
  json j;
  j["binsparse"]["version"] = version;
  j["binsparse"]["format"] = __detail::get_matrix_format_string(m);
  j["binsparse"]["shape"] = {m.m, m.n};
  j["binsparse"]["nnz"] = m.m * m.n;
  j["binsparse"]["data_types"]["values"] = type_info<T>::label();

  if (m.structure != general) {
    j["binsparse"]["structure"] =
        __detail::get_structure_name(m.structure).value();
  }

  for (auto&& v : user_keys.items()) {
    j[v.key()] = v.value();
  }

  hdf5_tools::set_attribute(f, "binsparse", j.dump(2));

  f.close();
}

template <typename T, typename I, typename Order,
          typename Allocator = std::allocator<T>>
auto read_dense_matrix(std::string fname, Allocator&& alloc = Allocator{}) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);

  auto metadata = hdf5_tools::get_attribute(f, "binsparse");

  using json = nlohmann::json;
  auto data = json::parse(metadata);

  std::cout << "Reading values...\n";
  auto binsparse_metadata = data["binsparse"];

  auto format = __detail::unalias_format(binsparse_metadata["format"]);

  assert(format ==
         __detail::get_matrix_format_string(dense_matrix<T, I, Order>{}));

  auto nrows = binsparse_metadata["shape"][0];
  auto ncols = binsparse_metadata["shape"][1];
  auto nnz = binsparse_metadata["nnz"];

  auto values = hdf5_tools::read_dataset<T>(f, "values", alloc);

  structure_t structure = general;

  if (binsparse_metadata.contains("structure")) {
    structure = __detail::parse_structure(binsparse_metadata["structure"]);
  }

  return dense_matrix<T, I, Order>{values.data(), nrows, ncols, structure};
}

// CSR Format

template <typename T, typename I>
void write_csr_matrix(std::string fname, csr_matrix<T, I> m,
                      nlohmann::json user_keys = {}) {

  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  std::span<T> values(m.values, m.nnz);
  std::span<I> colind(m.colind, m.nnz);
  std::span<I> row_ptr(m.row_ptr, m.m + 1);

  hdf5_tools::write_dataset(f, "values", values);
  hdf5_tools::write_dataset(f, "indices_1", colind);
  hdf5_tools::write_dataset(f, "pointers_to_1", row_ptr);

  using json = nlohmann::json;
  json j;
  j["binsparse"]["version"] = version;
  j["binsparse"]["format"] = "CSR";
  j["binsparse"]["shape"] = {m.m, m.n};
  j["binsparse"]["nnz"] = m.nnz;
  j["binsparse"]["data_types"]["pointers_to_1"] = type_info<I>::label();
  j["binsparse"]["data_types"]["indices_1"] = type_info<I>::label();
  j["binsparse"]["data_types"]["values"] = type_info<T>::label();

  if (m.structure != general) {
    j["binsparse"]["structure"] =
        __detail::get_structure_name(m.structure).value();
  }

  for (auto&& v : user_keys.items()) {
    j[v.key()] = v.value();
  }

  hdf5_tools::set_attribute(f, "binsparse", j.dump(2));

  f.close();
}

template <typename T, typename I, typename Allocator>
csr_matrix<T, I> read_csr_matrix(std::string fname, Allocator&& alloc) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);

  auto metadata = hdf5_tools::get_attribute(f, "binsparse");

  using json = nlohmann::json;
  auto data = json::parse(metadata);

  auto binsparse_metadata = data["binsparse"];

  assert(binsparse_metadata["format"] == "CSR");

  auto nrows = binsparse_metadata["shape"][0];
  auto ncols = binsparse_metadata["shape"][1];
  auto nnz = binsparse_metadata["nnz"];

  typename std::allocator_traits<
      std::remove_cvref_t<Allocator>>::template rebind_alloc<I>
      i_alloc(alloc);

  auto values = hdf5_tools::read_dataset<T>(f, "values", alloc);
  auto colind = hdf5_tools::read_dataset<I>(f, "indices_1", i_alloc);
  auto row_ptr = hdf5_tools::read_dataset<I>(f, "pointers_to_1", i_alloc);

  structure_t structure = general;

  if (binsparse_metadata.contains("structure")) {
    structure = __detail::parse_structure(binsparse_metadata["structure"]);
  }

  return csr_matrix<T, I>{values.data(), colind.data(), row_ptr.data(), nrows,
                          ncols,         nnz,           structure};
}

template <typename T, typename I>
csr_matrix<T, I> read_csr_matrix(std::string fname) {
  return read_csr_matrix<T, I>(fname, std::allocator<T>{});
}

// CSC Format

template <typename T, typename I>
void write_csc_matrix(std::string fname, csc_matrix<T, I> m,
                      nlohmann::json user_keys = {}) {

  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  std::span<T> values(m.values, m.nnz);
  std::span<I> rowind(m.rowind, m.nnz);
  std::span<I> col_ptr(m.col_ptr, m.m + 1);

  hdf5_tools::write_dataset(f, "values", values);
  hdf5_tools::write_dataset(f, "indices_1", rowind);
  hdf5_tools::write_dataset(f, "pointers_to_1", col_ptr);

  using json = nlohmann::json;
  json j;
  j["binsparse"]["version"] = version;
  j["binsparse"]["format"] = "CSR";
  j["binsparse"]["shape"] = {m.m, m.n};
  j["binsparse"]["nnz"] = m.nnz;
  j["binsparse"]["data_types"]["pointers_to_1"] = type_info<I>::label();
  j["binsparse"]["data_types"]["indices_1"] = type_info<I>::label();
  j["binsparse"]["data_types"]["values"] = type_info<T>::label();

  if (m.structure != general) {
    j["binsparse"]["structure"] =
        __detail::get_structure_name(m.structure).value();
  }

  for (auto&& v : user_keys.items()) {
    j[v.key()] = v.value();
  }

  hdf5_tools::set_attribute(f, "binsparse", j.dump(2));

  f.close();
}

template <typename T, typename I, typename Allocator>
csc_matrix<T, I> read_csc_matrix(std::string fname, Allocator&& alloc) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);

  auto metadata = hdf5_tools::get_attribute(f, "binsparse");

  using json = nlohmann::json;
  auto data = json::parse(metadata);

  auto binsparse_metadata = data["binsparse"];

  assert(binsparse_metadata["format"] == "CSC");

  auto nrows = binsparse_metadata["shape"][0];
  auto ncols = binsparse_metadata["shape"][1];
  auto nnz = binsparse_metadata["nnz"];

  typename std::allocator_traits<
      std::remove_cvref_t<Allocator>>::template rebind_alloc<I>
      i_alloc(alloc);

  auto values = hdf5_tools::read_dataset<T>(f, "values", alloc);
  auto rowind = hdf5_tools::read_dataset<I>(f, "indices_1", i_alloc);
  auto col_ptr = hdf5_tools::read_dataset<I>(f, "pointers_to_1", i_alloc);

  structure_t structure = general;

  if (binsparse_metadata.contains("structure")) {
    structure = __detail::parse_structure(binsparse_metadata["structure"]);
  }

  return csc_matrix<T, I>{values.data(), rowind.data(), col_ptr.data(), nrows,
                          ncols,         nnz,           structure};
}

template <typename T, typename I>
csc_matrix<T, I> read_csc_matrix(std::string fname) {
  return read_csc_matrix<T, I>(fname, std::allocator<T>{});
}

// COO Format

template <typename T, typename I>
void write_coo_matrix(std::string fname, coo_matrix<T, I> m,
                      nlohmann::json user_keys = {}) {

  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  std::span<T> values(m.values, m.nnz);
  std::span<I> rowind(m.rowind, m.nnz);
  std::span<I> colind(m.colind, m.nnz);

  hdf5_tools::write_dataset(f, "values", values);
  hdf5_tools::write_dataset(f, "indices_0", rowind);
  hdf5_tools::write_dataset(f, "indices_1", colind);

  using json = nlohmann::json;
  json j;
  j["binsparse"]["version"] = version;
  j["binsparse"]["format"] = "COO";
  j["binsparse"]["shape"] = {m.m, m.n};
  j["binsparse"]["nnz"] = m.nnz;
  j["binsparse"]["data_types"]["indices_0"] = type_info<I>::label();
  j["binsparse"]["data_types"]["indices_1"] = type_info<I>::label();
  j["binsparse"]["data_types"]["values"] = type_info<T>::label();

  if (m.structure != general) {
    j["binsparse"]["structure"] =
        __detail::get_structure_name(m.structure).value();
  }

  for (auto&& v : user_keys.items()) {
    j[v.key()] = v.value();
  }

  hdf5_tools::set_attribute(f, "binsparse", j.dump(2));

  f.close();
}

template <typename T, typename I, typename Allocator>
coo_matrix<T, I> read_coo_matrix(std::string fname, Allocator&& alloc) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);

  auto metadata = hdf5_tools::get_attribute(f, "binsparse");

  using json = nlohmann::json;
  auto data = json::parse(metadata);

  auto binsparse_metadata = data["binsparse"];

  auto format = __detail::unalias_format(binsparse_metadata["format"]);

  assert(format == "COOR" || format == "COOC");

  auto nrows = binsparse_metadata["shape"][0];
  auto ncols = binsparse_metadata["shape"][1];
  auto nnz = binsparse_metadata["nnz"];

  typename std::allocator_traits<
      std::remove_cvref_t<Allocator>>::template rebind_alloc<I>
      i_alloc(alloc);

  auto values = hdf5_tools::read_dataset<T>(f, "values", alloc);
  auto rows = hdf5_tools::read_dataset<I>(f, "indices_0", i_alloc);
  auto cols = hdf5_tools::read_dataset<I>(f, "indices_1", i_alloc);

  structure_t structure = general;

  if (binsparse_metadata.contains("structure")) {
    structure = __detail::parse_structure(binsparse_metadata["structure"]);
  }

  return coo_matrix<T, I>{values.data(), rows.data(), cols.data(), nrows,
                          ncols,         nnz,         structure};
}

template <typename T, typename I>
coo_matrix<T, I> read_coo_matrix(std::string fname) {
  return read_coo_matrix<T, I>(fname, std::allocator<T>{});
}

inline auto inspect(std::string fname) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);

  auto metadata = hdf5_tools::get_attribute(f, "binsparse");

  using json = nlohmann::json;
  auto data = json::parse(metadata);

  auto binsparse_metadata = data["binsparse"];

  assert(binsparse_metadata["version"] >= 0.1);

  return data;
}

} // namespace binsparse
