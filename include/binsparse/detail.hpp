#pragma once

#include <binsparse/containers/matrices.hpp>

namespace binsparse {

namespace __detail {

template <typename T, typename I>
inline std::string get_matrix_format_string(dense_matrix<T, I, row_major> m) {
  return "DMATR";
}

template <typename T, typename I>
inline std::string get_matrix_format_string(dense_matrix<T, I, column_major> m) {
  return "DMATC";
}

inline std::string unalias_format(const std::string& format) {
  if (format == "DMAT") {
    return "DMATR";
  }else if (format == "COO") {
    return "COOR";
  } else {
    return format;
  }
}

} // end __detail

} // end binsparse