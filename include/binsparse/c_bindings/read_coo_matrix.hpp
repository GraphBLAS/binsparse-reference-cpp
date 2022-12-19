#pragma once

#include <allocator_wrapper.hpp>

extern "C" {
  
template <typename T, typename I = std::size_t>
coo_matrix<T, I> read_coo_matrix(std::string fname) {
  return read_coo_matrix<T, I>(fname, std::allocator<T>{});
}

};