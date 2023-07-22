#pragma once

#include <binsparse/containers/matrices.hpp>
#include <nlohmann/json.hpp>
#include <binsparse/containers/matrices.hpp>
#include "hdf5_tools.hpp"
#include <memory>
#include <type_traits>

#include <binsparse/c_bindings/allocator_wrapper.hpp>

namespace binsparse {

// CSR Format

template <typename T, typename I>
void write_csr_matrix(std::string fname,
                      csr_matrix<T, I> m) {

  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  std::span<T> values(m.values, m.nnz);
  std::span<I> colind(m.colind, m.nnz);
  std::span<I> row_ptr(m.row_ptr, m.m+1);

  hdf5_tools::write_dataset(f, "values", values);
  hdf5_tools::write_dataset(f, "indices_1", colind);
  hdf5_tools::write_dataset(f, "pointers_to_1", row_ptr);

  std::string json_string =
  "{\n"
  "  \"format\": \"CSR\",\n"
  "  \"shape\": [";
  json_string += std::to_string(m.m) + ", " + std::to_string(m.n) +
  "],\n" +
  "  \"nnz\": " + std::to_string(m.nnz) + "\n" +
  "}\n";

  hdf5_tools::write_dataset(f, "metadata", json_string);

  f.close();
}

template <typename T, typename I, typename Allocator>
csr_matrix<T, I> read_csr_matrix(std::string fname, Allocator&& alloc) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);

  auto metadata = hdf5_tools::read_dataset<char>(f, "metadata");

  using json = nlohmann::json;
  auto data = json::parse(metadata);

  if (data["format"] == "CSR") {
    auto nrows = data["shape"][0];
    auto ncols = data["shape"][1];
    auto nnz = data["nnz"];

    typename std::allocator_traits<std::remove_cvref_t<Allocator>>
       :: template rebind_alloc<I> i_alloc(alloc);

    auto values = hdf5_tools::read_dataset<T>(f, "values", alloc);
    auto colind = hdf5_tools::read_dataset<I>(f, "indices_1", i_alloc);
    auto row_ptr = hdf5_tools::read_dataset<I>(f, "pointers_to_1", i_alloc);

    return csr_matrix<T, I>{values.data(), colind.data(), row_ptr.data(), nrows, ncols, nnz};
  } else {
    assert(false);
  }
}

template <typename T, typename I>
csr_matrix<T, I> read_csr_matrix(std::string fname) {
  return read_csr_matrix<T, I>(fname, std::allocator<T>{});
}

// COO Format

template <typename T, typename I>
void write_coo_matrix(std::string fname,
                      coo_matrix<T, I> m) {

  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  std::span<T> values(m.values, m.nnz);
  std::span<I> rowind(m.rowind, m.nnz);
  std::span<I> colind(m.colind, m.nnz);

  hdf5_tools::write_dataset(f, "values", values);
  hdf5_tools::write_dataset(f, "indices_0", rowind);
  hdf5_tools::write_dataset(f, "indices_1", colind);

  std::string json_string =
  "{\n"
  "  \"format\": \"COO\",\n"
  "  \"shape\": [";
  json_string += std::to_string(m.m) + ", " + std::to_string(m.n) +
  "],\n" +
  "  \"nnz\": " + std::to_string(m.nnz) + "\n" +
  "}\n";

  hdf5_tools::write_dataset(f, "metadata", json_string);

  f.close();
}

template <typename T, typename I, typename Allocator>
coo_matrix<T, I> read_coo_matrix(std::string fname, Allocator&& alloc) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);

  auto metadata = hdf5_tools::read_dataset<char>(f, "metadata");

  using json = nlohmann::json;
  auto data = json::parse(metadata);

  if (data["format"] == "COO") {
    auto nrows = data["shape"][0];
    auto ncols = data["shape"][1];
    auto nnz = data["nnz"];

    typename std::allocator_traits<std::remove_cvref_t<Allocator>>
       :: template rebind_alloc<I> i_alloc(alloc);

    auto values = hdf5_tools::read_dataset<T>(f, "values", alloc);
    auto rows = hdf5_tools::read_dataset<I>(f, "indices_0", i_alloc);
    auto cols = hdf5_tools::read_dataset<I>(f, "indices_1", i_alloc);

    return coo_matrix<T, I>{values.data(), rows.data(), cols.data(), nrows, ncols, nnz};
  } else {
    assert(false);
  }
}

template <typename T, typename I>
coo_matrix<T, I> read_coo_matrix(std::string fname) {
  return read_coo_matrix<T, I>(fname, std::allocator<T>{});
}

} // end binsparse

#include <binsparse/c_bindings/bc_read_matrix.hpp>
