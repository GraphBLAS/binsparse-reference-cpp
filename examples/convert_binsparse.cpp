#include <binsparse/binsparse.hpp>
#include <iostream>
#include <concepts>
#include <complex>

template <typename T, typename I>
void convert_to_binsparse(std::string input_file, std::string output_file, std::string format, std::string comment) {
  nlohmann::json user_keys;
  user_keys["comment"] = comment;
  if (format == "CSR") {
    std::cout << "Reading in " << input_file << "...\n";
    auto x = binsparse::__detail::mmread<T, I, binsparse::__detail::csr_matrix_owning<T, I>>(input_file);
    binsparse::csr_matrix<T, I> matrix{x.values().data(), x.colind().data(), x.rowptr().data(), std::get<0>(x.shape()), std::get<1>(x.shape()), I(x.size())};
    binsparse::write_csr_matrix(output_file, matrix, user_keys);
    std::cout << "Writing to binsparse file " << output_file << " using " << format << " format...\n";
  } else {
    auto x = binsparse::__detail::mmread<T, I, binsparse::__detail::coo_matrix_owning<T, I>>(input_file);
    binsparse::coo_matrix<T, I> matrix{x.values().data(), x.rowind().data(), x.colind().data(), std::get<0>(x.shape()), std::get<1>(x.shape()), I(x.size())};
    binsparse::write_coo_matrix(output_file, matrix, user_keys);
    std::cout << "Writing to binsparse file " << output_file << " using " << format << " format...\n";
  }
}

template <typename I>
void convert_to_binsparse(std::string input_file, std::string output_file, std::string type,
             std::string format, std::string comment) {
  if (type == "real") {
    convert_to_binsparse<float, I>(input_file, output_file, format, comment);
  } else if (type == "complex") {
    assert(false);
    // convert_to_binsparse<std::complex<float>, I>(input_file, output_file, format, comment);
  } else if (type == "integer") {
    convert_to_binsparse<int64_t, I>(input_file, output_file, format, comment);
  } else if (type == "pattern") {
    convert_to_binsparse<uint8_t, I>(input_file, output_file, format, comment);
  }
}


int main(int argc, char** argv) {

  if (argc < 3) {
    std::cout << "usage: ./convert_binsparse [input_file.mtx] [output_file.hdf5] [optional: format {CSR, COO}]\n";
    return 1;
  }

  std::string input_file(argv[1]);
  std::string output_file(argv[2]);

  std::string format;

  if (argc >= 4) {
    format = argv[3];

    for (auto&& c : format) {
      c = std::toupper(c);
    }
  } else {
    format = "COO";
  }

  auto [m, n, nnz, type, comment] = binsparse::mmread_metadata(input_file);

  std::cout << "Matrix is " << m << " x " << n << " with " << nnz << " values.\n";
  std::cout << "Type: " << type << std::endl;
  std::cout << "Comment:\n";
  std::cout << comment;

  assert(format == "COO" || format == "CSR");

  auto max_size = std::max({m, n, nnz});

  if (max_size + 1 <= std::numeric_limits<uint8_t>::max()) {
    convert_to_binsparse<uint8_t>(input_file, output_file, type, format, comment);
  } else if (max_size + 1 <= std::numeric_limits<uint16_t>::max()) {
    convert_to_binsparse<uint16_t>(input_file, output_file, type, format, comment);
  } else if (max_size + 1 <= std::numeric_limits<uint32_t>::max()) {
    convert_to_binsparse<uint32_t>(input_file, output_file, type, format, comment);
  } else if (max_size + 1 <= std::numeric_limits<uint64_t>::max()) {
    convert_to_binsparse<uint64_t>(input_file, output_file, type, format, comment);
  } else {
    throw std::runtime_error("Error! Matrix dimensions or NNZ too large to handle.");
  }

  return 0;
}
