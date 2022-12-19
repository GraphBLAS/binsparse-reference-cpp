#include <iostream>
#include <binsparse/binsparse.hpp>

int main(int argc, char** argv) {
  printf("Hello, world!\n");
  auto mat = binsparse::read_coo_matrix<float>("data/matrix.hdf5");

  for (size_t i = 0; i < mat.nnz; i++) {
    std::cout << mat.rowind[i] << ", " << mat.colind[i] << ": " << mat.values[i] << std::endl;
  }

  return 0;
}
