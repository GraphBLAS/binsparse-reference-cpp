#include "util.hpp"
#include <binsparse/binsparse.hpp>
#include <filesystem>
#include <fmt/core.h>
#include <gtest/gtest.h>

TEST(BinsparseReadWrite, COOFormat) {
  using T = float;
  using I = std::size_t;

  std::string binsparse_file = "out.bsp.hdf5";

  auto base_path = find_prefix(files.front());

  for (auto&& file : files) {
    auto file_path = base_path + file;
    auto x = binsparse::__detail::mmread<
        T, I, binsparse::__detail::coo_matrix_owning<T, I>>(file_path);

    auto&& [num_rows, num_columns] = x.shape();
    binsparse::coo_matrix<T, I> matrix{x.values().data(), x.rowind().data(),
                                       x.colind().data(), num_rows,
                                       num_columns,       I(x.size())};
    binsparse::write_coo_matrix(binsparse_file, matrix);

    auto matrix_ = binsparse::read_coo_matrix<T, I>(binsparse_file);

    EXPECT_EQ(matrix.nnz, matrix_.nnz);
    EXPECT_EQ(matrix.m, matrix_.m);
    EXPECT_EQ(matrix.n, matrix_.n);

    for (I i = 0; i < matrix.nnz; i++) {
      EXPECT_EQ(matrix.values[i], matrix_.values[i]);
    }

    for (I i = 0; i < matrix.nnz; i++) {
      EXPECT_EQ(matrix.rowind[i], matrix_.rowind[i]);
    }

    for (I i = 0; i < matrix.nnz; i++) {
      EXPECT_EQ(matrix.colind[i], matrix_.colind[i]);
    }

    delete matrix_.values;
    delete matrix_.rowind;
    delete matrix_.colind;
  }

  std::filesystem::remove(binsparse_file);
}
