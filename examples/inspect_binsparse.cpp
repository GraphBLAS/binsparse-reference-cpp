#include <binsparse/binsparse.hpp>
#include <iostream>
#include <concepts>
#include <complex>

int main(int argc, char** argv) {

  if (argc < 2) {
    std::cout << "usage: ./inspect_binsparse [input_file.mtx]\n";
    return 1;
  }

  std::string input_file(argv[1]);

  auto j = binsparse::inspect(input_file);
  auto metadata = j["binsparse"];

  std::cout << "Inspecting Binsparse v" << metadata["version"] << " file...\n";
  std::cout << metadata["format"] << " format matrix of dimension "
            << metadata["shape"] << " with " << metadata["nnz"] << " nonzeros\n";

  if (metadata["format"] == "COO") {
    auto i0 = metadata["data_types"]["indices_0"];
    auto i1 = metadata["data_types"]["indices_1"];
    auto t = metadata["data_types"]["values"];

    std::cout << "Stored using index types: " << i0 << " " << i1 << std::endl;
    std::cout << "Value type: " << t << std::endl;
  } else if (metadata["format"] == "CSR") {
    auto i0 = metadata["data_types"]["pointers_to_1"];
    auto i1 = metadata["data_types"]["indices_1"];
    auto t = metadata["data_types"]["values"];
    std::cout << "Stored using index types: " << i0 << " " << i1 << std::endl;
    std::cout << "Value type: " << t << std::endl;
  } else {
    assert(false);
  }

  std::cout << "Raw JSON:\n";
  std::cout << j.dump(2) << std::endl;

  return 0;
}
