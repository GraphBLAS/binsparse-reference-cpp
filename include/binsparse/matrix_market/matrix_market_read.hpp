#pragma once

#include <iostream>
#include <fstream>
#include <ranges>

namespace binsparse {

namespace __detail {

template <typename T, typename I>
class csr_matrix_owning {
public:

  csr_matrix_owning(std::tuple<I, I> shape) : shape_(shape) {}

  auto values() { return std::ranges::views::all(values_); }
  auto rowptr() { return std::ranges::views::all(rowptr_); }
  auto colind() { return std::ranges::views::all(colind_); }

  auto values() const { return std::ranges::views::all(values_); }
  auto rowptr() const { return std::ranges::views::all(rowptr_); }
  auto colind() const { return std::ranges::views::all(colind_); }

  template <typename Iter>
  void assign_tuples(Iter first, Iter last) {
    std::size_t nnz = std::ranges::distance(first, last);
    values_.resize(nnz);
    colind_.resize(nnz);
    rowptr_.resize(std::get<0>(shape())+1);

    rowptr_[0] = 0;
    
    std::size_t r = 0;
    std::size_t c = 0;
    for (auto iter = first; iter != last; ++iter) {
      auto&& [index, value] = *iter;
      auto&& [i, j] = index;

      values_[c] = value;
      colind_[c] = j;

      while (r < i) {
        if (r+1 > std::get<0>(shape())) {
          // TODO: exception?
          // throw std::runtime_error("csr_matrix_impl_: given invalid matrix");
        }
        rowptr_[r+1] = c;
        r++;
      }
      c++;

      if (c > nnz) {
        // TODO: exception?
        // throw std::runtime_error("csr_matrix_impl_: given invalid matrix");
      }
    }

    for ( ; r < std::get<0>(shape()); r++) {
      rowptr_[r+1] = nnz;
    }
  }

  auto shape() const {
    return shape_;
  }

  auto size() const {
    return values_.size();
  }

private:
  std::tuple<I, I> shape_;
  std::vector<T> values_;
  std::vector<I> rowptr_;
  std::vector<I> colind_;
};

template <typename T, typename I>
class coo_matrix_owning {
public:

  coo_matrix_owning(std::tuple<I, I> shape) : shape_(shape) {}

  auto values() { return std::ranges::views::all(values_); }
  auto rowind() { return std::ranges::views::all(rowind_); }
  auto colind() { return std::ranges::views::all(colind_); }

  auto values() const { return std::ranges::views::all(values_); }
  auto rowind() const { return std::ranges::views::all(rowind_); }
  auto colind() const { return std::ranges::views::all(colind_); }

  void push_back(std::tuple<std::tuple<I, I>, T> entry) {
    auto&& [idx, v] = entry;
    auto&& [i, j] = idx;
    values_.push_back(v);
    rowind_.push_back(i);
    colind_.push_back(j);
  }

  template <typename Iter>
  void assign_tuples(Iter first, Iter last) {
    std::size_t nnz = std::ranges::distance(first, last);
    for (auto iter = first; iter != last; ++iter) {
      auto&& [idx, v] = *iter;
      auto&& [i, j] = idx;
      push_back({{i, j}, v});
    }
  }

  void reserve(std::size_t size) {
    values_.reserve(size);
    rowind_.reserve(size);
    colind_.reserve(size);
  }

  auto shape() const {
    return shape_;
  }

  auto size() const {
    return values_.size();
  }

private:
  std::tuple<I, I> shape_;
  std::vector<T> values_;
  std::vector<I> rowind_;
  std::vector<I> colind_;
};

/// Read in the Matrix Market file at location `file_path` and
/// return a data structure with the matrix.
template <typename T, typename I, typename MatrixType>
inline MatrixType mmread(std::string file_path, bool one_indexed = true) {
  using index_type = I;
  using size_type = std::size_t;

  std::ifstream f;

  f.open(file_path.c_str());

  if (!f.is_open()) {
    // TODO better choice of exception.
    throw std::runtime_error("mmread: cannot open " + file_path);
  }

  std::string buf;

  // Make sure the file is matrix market matrix, coordinate, and check whether
  // it is symmetric. If the matrix is symmetric, non-diagonal elements will
  // be inserted in both (i, j) and (j, i).  Error out if skew-symmetric or
  // Hermitian.
  std::getline(f, buf);
  std::istringstream ss(buf);
  std::string item;
  ss >> item;
  if (item != "%%MatrixMarket") {
    throw std::runtime_error(file_path + " could not be parsed as a Matrix Market file.");
  }
  ss >> item;
  if (item != "matrix") {
    throw std::runtime_error(file_path + " could not be parsed as a Matrix Market file.");
  }
  ss >> item;
  if (item != "coordinate") {
    throw std::runtime_error(file_path + " could not be parsed as a Matrix Market file.");
  }
  bool pattern;
  ss >> item;
  if (item == "pattern") {
    pattern = true;
  } else {
    pattern = false;
  }
  // TODO: do something with real vs. integer vs. pattern?
  ss >> item;
  bool symmetric;
  if (item == "general") {
    symmetric = false;
  } else if (item == "symmetric") {
    symmetric = true;
  } else {
    throw std::runtime_error(file_path + " has an unsupported matrix type");
  }

  bool outOfComments = false;
  while (!outOfComments) {
    std::getline(f, buf);

    if (buf[0] != '%') {
      outOfComments = true;
    }
  }

  I m, n, nnz;
  // std::istringstream ss(buf);
  ss.clear();
  ss.str(buf);
  ss >> m >> n >> nnz;

  // NOTE for symmetric matrices: `nnz` holds the number of stored values in
  // the matrix market file, while `matrix.nnz_` will hold the total number of
  // stored values (including "mirrored" symmetric values).
  MatrixType m_out({m, n});

  using coo_type = std::vector<std::tuple<std::tuple<I, I>, T>>;
  coo_type matrix;
  if (symmetric) {
    matrix.reserve(2*nnz);
  } else {
    matrix.reserve(nnz);
  }

  size_type c = 0;
  while (std::getline(f, buf)) {
    I i, j;
    T v;
    std::istringstream ss(buf);
    if (!pattern) {
      ss >> i >> j >> v;
    } else {
      ss >> i >> j;
      v = T(1);
    }
    if (one_indexed) {
      i--;
      j--;
    }

    if (i >= m || j >= n) {
      throw std::runtime_error("read_MatrixMarket: file has nonzero out of bounds.");
    }

    matrix.push_back({{i, j}, v});

    if (symmetric && i != j) {
      matrix.push_back({{j, i}, v});
    }

    c++;
    if (c > nnz) {
      throw std::runtime_error("read_MatrixMarket: error reading Matrix Market file, file has more nonzeros than reported.");
    }
  }

  auto sort_fn = [](auto&& a, auto&& b) {
                   auto&& [a_idx, a_v] = a;
                   auto&& [b_idx, b_v] = b;

                   auto&& [a_i, a_j] = a_idx;
                   auto&& [b_i, b_j] = b_idx;

                   if (a_i != b_i) {
                    return a_i < b_i;
                   } else {
                    return a_j < b_j;
                   }
                 };

  std::sort(matrix.begin(), matrix.end(), sort_fn);

  m_out.assign_tuples(matrix.begin(), matrix.end());

  f.close();

  return m_out;
}

} // end __detail

} // end binsparse