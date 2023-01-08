#include <binsparse/binsparse.hpp>
#include <iostream>

int main(int argc, char** argv) {

  binsparse::allocator_wrapper<float> alloc(malloc, free);

  bc_read_matrix("new_matrix.hdf5");

  return 0;
}
