#include <binsparse/binsparse.hpp>
#include <complex>
#include <concepts>
#include <iostream>

int main(int argc, char** argv) {

  if (argc < 2) {
    std::cout << "usage: ./inspect_binsparse [input_file.mtx]\n";
    return 1;
  }

  std::string input_file(argv[1]);

  auto metadata = binsparse::inspect(input_file);

  std::cout << "Inspecting Binsparse v" << metadata["version"] << " file...\n";
  std::cout << metadata["format"] << " format matrix of dimension "
            << metadata["shape"] << " with " << metadata["nnz"]
            << " nonzeros\n";

  if (metadata["format"] == "COO") {
    auto i0 = metadata["data_types"]["indices_0"];
    auto i1 = metadata["data_types"]["indices_1"];
    auto t = metadata["data_types"]["values"];

    binsparse::visit_label(
        {i0, i1, t},
        [&]<typename I1, typename I2, typename T>(I1 i, I2 j, T v) {
          using I = std::conditional_t<std::numeric_limits<I1>::max() <
                                           std::numeric_limits<I2>::max(),
                                       I2, I1>;
          std::cout << "Reading binsparse with index and value types: "
                    << binsparse::type_info<I>::label() << " "
                    << binsparse::type_info<T>::label() << "\n";

          auto m = binsparse::read_coo_matrix<T, I>(input_file);
        });
  } else if (metadata["format"] == "CSR") {
    auto i0 = metadata["data_types"]["pointers_to_1"];
    auto i1 = metadata["data_types"]["indices_1"];
    auto t = metadata["data_types"]["values"];

    binsparse::visit_label(
        {i0, i1, t},
        [&]<typename I1, typename I2, typename T>(I1 i, I2 j, T v) {
          using I = std::conditional_t<std::numeric_limits<I1>::max() <
                                           std::numeric_limits<I2>::max(),
                                       I2, I1>;
          std::cout << "Reading binsparse with index and value types: "
                    << binsparse::type_info<I>::label() << " "
                    << binsparse::type_info<T>::label() << "\n";

          auto m = binsparse::read_csr_matrix<T, I>(input_file);
        });
  } else {
    assert(false);
  }

  return 0;
}
