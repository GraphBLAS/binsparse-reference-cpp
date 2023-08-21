#include <binsparse/binsparse.hpp>
#include <grb/grb.hpp>
#include <iostream>
#include <concepts>
#include <complex>

template <typename T, typename I>
void convert(std::string input_file, std::string output_file, std::string format, std::string comment) {
  if (format == "CSR") {
    std::cout << "Reading in " << input_file << "...\n";
    grb::matrix<T, I> x("mouse_gene.mtx");
    binsparse::csr_matrix<T, I> matrix{x.backend_.values_.data(), x.backend_.colind_.data(), x.backend_.rowptr_.data(), x.shape()[0], x.shape()[1], I(x.size())};
    binsparse::write_csr_matrix(output_file, matrix);
    std::cout << "Writing to binsparse file " << output_file << " using " << format << " format...\n";
  } else {
    assert(false);
    /*
    std::cout << "Reading in " << input_file << "...\n";
    grb::matrix<T, I, grb::coordinate> x("mouse_gene.mtx");
    binsparse::coo_matrix<T, I> matrix{x.backend_.values_.data(), x.backend_.rowptr_.data(), x.backend_.colind_.data(), x.shape()[0], x.shape()[1], I(x.size())};
    binsparse::write_coo_matrix(output_file, matrix);
    std::cout << "Writing to binsparse file " << output_file << " using " << format << " format...\n";
    */
  }
}

template <typename I>
void convert(std::string input_file, std::string output_file, std::string type,
             std::string format, std::string comment) {
  if (type == "real") {
    convert<float, I>(input_file, output_file, format, comment);
  } else if (type == "complex") {
    assert(false);
    // convert<std::complex<float>, I>(input_file, output_file, format, comment);
  } else if (type == "integer") {
    convert<int64_t, I>(input_file, output_file, format, comment);
  } else if (type == "pattern") {
    convert<bool, I>(input_file, output_file, format, comment);
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
    format = "CSR";
  }

  auto [m, n, nnz, type, comment] = binsparse::mmread_metadata(input_file);

  std::cout << "Matrix is " << m << " x " << n << " with " << nnz << " values.\n";
  std::cout << "Type: " << type << std::endl;
  std::cout << "Comment:\n";
  std::cout << comment;

  assert(format == "COO" || format == "CSR");

  auto max_size = std::max({m, n, nnz});

  if (max_size + 1 <= std::numeric_limits<uint8_t>::max()) {
    convert<uint8_t>(input_file, output_file, type, format, comment);
  } else if (max_size + 1 <= std::numeric_limits<uint16_t>::max()) {
    convert<uint16_t>(input_file, output_file, type, format, comment);
  } else if (max_size + 1 <= std::numeric_limits<uint32_t>::max()) {
    convert<uint32_t>(input_file, output_file, type, format, comment);
  } else if (max_size + 1 <= std::numeric_limits<uint64_t>::max()) {
    convert<uint64_t>(input_file, output_file, type, format, comment);
  } else {
    throw std::runtime_error("Error! Matrix dimensions or NNZ too large to handle.");
  }

  return 0;
}
