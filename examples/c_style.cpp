#include <binsparse/binsparse.hpp>
#include <iostream>

int main(int argc, char** argv) {

  binsparse::allocator_wrapper<float> alloc(malloc, free);

  auto mat = binsparse::read_coo_matrix<float, std::size_t>("data/matrix.hdf5", alloc);

  for (size_t i = 0; i < mat.nnz; i++) {
    std::cout << mat.rowind[i] << ", " << mat.colind[i] << ": " << mat.values[i] << std::endl;
  }

  // binsparse::write_coo_matrix("new_matrix.hdf5", mat);

  return 0;
}
