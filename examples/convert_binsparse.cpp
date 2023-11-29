#include <binsparse/binsparse.hpp>
#include <complex>
#include <concepts>
#include <iostream>

template <typename T, typename I>
void convert_to_binsparse(std::string input_file, std::string output_file,
                          std::string format, std::string comment,
                          std::optional<std::string> group = {}) {
  H5::H5File file;
  std::unique_ptr<H5::Group> f_p;

  if (!group.has_value()) {
    f_p = std::unique_ptr<H5::Group>(
        new H5::H5File(output_file.c_str(), H5F_ACC_TRUNC));
  } else {
    file = H5::H5File(output_file.c_str(), H5F_ACC_RDWR);
    H5::Group g = file.createGroup(group.value().c_str());
    f_p = std::unique_ptr<H5::Group>(new H5::Group(g));
  }

  H5::Group& f = *f_p;

  nlohmann::json user_keys;
  user_keys["comment"] = comment;
  if (format == "CSR") {
    auto x = binsparse::__detail::mmread<
        T, I, binsparse::__detail::csr_matrix_owning<T, I>>(input_file);
    binsparse::csr_matrix<T, I> matrix{
        x.values().data(),      x.colind().data(),      x.rowptr().data(),
        std::get<0>(x.shape()), std::get<1>(x.shape()), I(x.size()),
        x.structure()};
    binsparse::write_csr_matrix(f, matrix, user_keys);
    std::cout << "Writing to binsparse file " << output_file << " using "
              << format << " format...\n";
  } else {
    auto x = binsparse::__detail::mmread<
        T, I, binsparse::__detail::coo_matrix_owning<T, I>>(input_file);
    binsparse::coo_matrix<T, I> matrix{
        x.values().data(),      x.rowind().data(),      x.colind().data(),
        std::get<0>(x.shape()), std::get<1>(x.shape()), I(x.size()),
        x.structure()};
    binsparse::write_coo_matrix(f, matrix, user_keys);
    std::cout << "Writing to binsparse file " << output_file << " using "
              << format << " format...\n";
  }
}

template <typename I>
void convert_to_binsparse(std::string input_file, std::string output_file,
                          std::string type, std::string format,
                          std::string comment,
                          std::optional<std::string> group = {}) {
  if (type == "real") {
    convert_to_binsparse<float, I>(input_file, output_file, format, comment,
                                   group);
  } else if (type == "complex") {
    assert(false);
    // convert_to_binsparse<std::complex<float>, I>(input_file, output_file,
    // format, comment);
  } else if (type == "integer") {
    convert_to_binsparse<int64_t, I>(input_file, output_file, format, comment,
                                     group);
  } else if (type == "pattern") {
    convert_to_binsparse<uint8_t, I>(input_file, output_file, format, comment,
                                     group);
  }
}

template <typename T>
void convert_to_binsparse_vector(std::string input_file,
                                 std::string output_file, std::string type,
                                 std::string comment,
                                 std::optional<std::string> group) {
  H5::H5File file;
  std::unique_ptr<H5::Group> f_p;

  if (!group.has_value()) {
    f_p = std::unique_ptr<H5::Group>(
        new H5::H5File(output_file.c_str(), H5F_ACC_TRUNC));
  } else {
    file = H5::H5File(output_file.c_str(), H5F_ACC_RDWR);
    H5::Group g = file.createGroup(group.value().c_str());
    f_p = std::unique_ptr<H5::Group>(new H5::Group(g));
  }

  H5::Group& f = *f_p;

  nlohmann::json user_keys;
  user_keys["comment"] = comment;

  auto x = binsparse::__detail::mmread_array<float>(input_file);
  binsparse::write_dense_vector(f, std::span(x), user_keys);
  std::cout << "Writing to binsparse file " << output_file << " as vector"
            << std::endl;
}

inline void convert_to_binsparse_vector(std::string input_file,
                                        std::string output_file,
                                        std::string type, std::string comment,
                                        std::optional<std::string> group = {}) {
  if (type == "real") {
    convert_to_binsparse_vector<float>(input_file, output_file, type, comment,
                                       group);
  } else if (type == "integer") {
    convert_to_binsparse_vector<int64_t>(input_file, output_file, type, comment,
                                         group);
  } else {
    throw std::runtime_error("convert_to_binsparse_vector: unsupported type");
  }
}

int main(int argc, char** argv) {

  if (argc < 3) {
    std::cout << "usage: ./convert_binsparse [input_file.mtx] "
                 "[output_file.hdf5] [optional: format {CSR, COO}] [optional: "
                 "HDF5 group name]\n";
    return 1;
  }

  std::string input_file(argv[1]);
  std::string output_file(argv[2]);

  std::string format;
  std::optional<std::string> group;

  if (argc >= 4) {
    format = argv[3];

    for (auto&& c : format) {
      c = std::toupper(c);
    }
  } else {
    format = "COO";
  }

  if (argc >= 5) {
    group = argv[4];
  }

  auto [m, n, nnz, mm_format, type, structure, comment] =
      binsparse::mmread_metadata(input_file);

  if (mm_format == "coordinate") {
    std::cout << "Matrix is " << m << " x " << n << " with " << nnz
              << " values.\n";
    std::cout << "Type: " << type << std::endl;
    std::cout << "Structure: " << structure << std::endl;
    std::cout << "Comment:\n";
    std::cout << comment;

    assert(format == "COO" || format == "CSR");

    auto max_size = std::max({m, n, nnz});

    if (max_size + 1 <= std::numeric_limits<uint8_t>::max()) {
      convert_to_binsparse<uint8_t>(input_file, output_file, type, format,
                                    comment, group);
    } else if (max_size + 1 <= std::numeric_limits<uint16_t>::max()) {
      convert_to_binsparse<uint16_t>(input_file, output_file, type, format,
                                     comment, group);
    } else if (max_size + 1 <= std::numeric_limits<uint32_t>::max()) {
      convert_to_binsparse<uint32_t>(input_file, output_file, type, format,
                                     comment, group);
    } else if (max_size + 1 <= std::numeric_limits<uint64_t>::max()) {
      convert_to_binsparse<uint64_t>(input_file, output_file, type, format,
                                     comment, group);
    } else {
      throw std::runtime_error(
          "Error! Matrix dimensions or NNZ too large to handle.");
    }
  } else if (mm_format == "array" && n == 1) {
    convert_to_binsparse_vector(input_file, output_file, type, comment, group);
  } else {
    throw std::runtime_error("Encountered unsupported MatrixMarket format");
  }

  return 0;
}
