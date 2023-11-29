#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace binsparse {

// Read metadata from Matrix Market file.
// Returns a tuple holding 5 values.
// 0 - number of rows in matrix
// 1 - number of columns in matrix
// 2 - number of values in matrix
// 3 - type of the matrix (real / integer / complex / pattern)
// 4 - structure of the matrix (general / symmetric / skew-symmetric /
// Hermitian) 5 - comments
inline auto mmread_metadata(std::string file_path) {
  std::string type, structure;

  std::ifstream f;

  f.open(file_path.c_str());

  if (!f.is_open()) {
    // TODO better choice of exception.
    throw std::runtime_error("mmread_metadata: cannot open " + file_path);
  }

  std::string buf;

  // Read in first line.
  std::getline(f, buf);
  std::istringstream ss(buf);

  std::string item;

  // Check file is Matrix Market format.
  ss >> item;
  if (item != "%%MatrixMarket") {
    throw std::runtime_error(file_path +
                             " could not be parsed as a Matrix Market file.");
  }

  // Read in "matrix"
  ss >> item;
  if (item != "matrix") {
    throw std::runtime_error(file_path +
                             " could not be parsed as a Matrix Market file.");
  }

  // Read in coordinate / array
  ss >> item;
  std::string format = item;

  // Read in type of matrix (real / integer / complex / pattern)
  ss >> item;
  type = item;

  // Read in general / symmetric / skew-symmetric / Hermitian
  ss >> item;
  structure = item;

  std::string comment;

  bool outOfComments = false;
  while (!outOfComments) {
    std::getline(f, buf);

    comment += buf + "\n";

    if (buf[0] != '%') {
      outOfComments = true;
    }
  }

  std::size_t m, n, nnz;

  ss.clear();
  ss.str(buf);
  ss >> m >> n;
  if (format == "coordinate") {
    ss >> nnz;
  } else {
    nnz = m * n;
  }

  return std::tuple(m, n, nnz, format, type, structure, comment);
}

} // namespace binsparse
