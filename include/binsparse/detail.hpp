#pragma once

#include <binsparse/containers/matrices.hpp>

namespace binsparse {

namespace __detail {

template <typename T, typename I>
inline std::string get_matrix_format_string(dense_matrix<T, I, row_major> m) {
  return "DMATR";
}

template <typename T, typename I>
inline std::string
get_matrix_format_string(dense_matrix<T, I, column_major> m) {
  return "DMATC";
}

inline std::string unalias_format(const std::string& format) {
  if (format == "DMAT") {
    return "DMATR";
  } else if (format == "COO") {
    return "COOR";
  } else {
    return format;
  }
}

inline std::optional<std::string> get_structure_name(structure_t structure) {
  if (structure == general) {
    return {};
  } else if (structure == symmetric) {
    return "symmetric_lower";
  } else if (structure == skew_symmetric) {
    return "skew_symmetric_lower";
  } else if (structure == hermitian) {
    return "hermitian";
  } else {
    throw std::runtime_error("get_structure_name: unknown structure");
  }
}

inline structure_t parse_structure(const std::string& structure) {
  if (structure == "symmetric_lower") {
    return symmetric;
  } else if (structure == "skew_symmetric_lower") {
    return skew_symmetric;
  } else if (structure == "hermitian") {
    return hermitian;
  } else {
    throw std::runtime_error("parse_structure: unsupported structure");
  }
}

} // namespace __detail

} // namespace binsparse
