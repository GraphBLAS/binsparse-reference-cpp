#pragma once

#include <binsparse/containers/matrices.hpp>
#include <nlohmann/json.hpp>
#include <binsparse/containers/matrices.hpp>
#include "hdf5_tools.hpp"
#include <memory>

namespace binsparse {

/*
template <typename T, typename I>
void write_matrix(std::string fname,
                  const std::vector<std::tuple<std::tuple<I, I>, T>>& tuples) {

  using value_type = T
  using index_type = I

  std::vector<std::ranges::range_value_t<M>> tuples(matrix.begin(), matrix.end());

  auto sort_fn = [](const auto& a, const auto& b) {
                   auto&& [a_index, a_value] = a;
                   auto&& [b_index, b_value] = b;
                   auto&& [a_i, a_j] = a_index;
                   auto&& [b_i, b_j] = b_index;
                   if (a_i < b_i) {
                     return true;
                   }
                   else if (a_i == b_i) {
                     if (a_j < b_j) {
                      return true;
                     }
                   }
                   return false;
                 };

  std::sort(tuples.begin(), tuples.end(), sort_fn);

  std::vector<grb::matrix_index_t<M>> rows;
  std::vector<grb::matrix_index_t<M>> cols;
  std::vector<grb::matrix_scalar_t<M>> vals;

  rows.reserve(matrix.size());
  cols.reserve(matrix.size());
  vals.reserve(matrix.size());

  for (auto&& [index, value] : tuples) {
    auto&& [row, col] = index;
    rows.push_back(row);
    cols.push_back(col);
    vals.push_back(value);
  }

  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  hdf5_tools::write_dataset(f, "values", vals);
  hdf5_tools::write_dataset(f, "indices_0", rows);
  hdf5_tools::write_dataset(f, "indices_1", cols);

  std::string json_string =
  "{\n"
  "  \"format\": \"COO\",\n"
  "  \"shape\": [";
  json_string += std::to_string(matrix.shape()[0]) + ", " + std::to_string(matrix.shape()[1]) +
  "],\n" +
  "  \"nnz\": " + std::to_string(matrix.size()) + "\n" +
  "}\n";

  hdf5_tools::write_dataset(f, "metadata", json_string);

  f.close();
}
*/

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

    typename std::allocator_traits<Allocator>:: template rebind_alloc<I> i_alloc(alloc);

    auto values = hdf5_tools::read_dataset<T>(f, "values", alloc);
    auto rows = hdf5_tools::read_dataset<I>(f, "indices_0", i_alloc);
    auto cols = hdf5_tools::read_dataset<I>(f, "indices_1", i_alloc);

    return coo_matrix<T, I>{values.data(), rows.data(), cols.data(), nrows, ncols, nnz};
  } else {
    assert(false);
  }
}

template <typename T, typename I = std::size_t>
coo_matrix<T, I> read_coo_matrix(std::string fname) {
  return read_coo_matrix<T, I>(fname, std::allocator<T>{});
}

} // end binsparse
