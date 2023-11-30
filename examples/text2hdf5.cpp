#include <binsparse/binsparse.hpp>
#include <complex>
#include <concepts>
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {

  if (argc < 4) {
    std::cout << "usage: ./write_text_file [input_file.txt] "
                 "[output_file.hdf5] [HDF5 Dataset Name]\n";
    return 1;
  }

  std::string input_file(argv[1]);
  std::string output_file(argv[2]);
  std::string dataset_name(argv[3]);

  std::cout << "Read in text file " << input_file << " write to dataset "
            << dataset_name << " in file " << output_file << std::endl;

  std::ifstream f(input_file);
  std::stringstream ss;
  ss << f.rdbuf();

  std::string file_contents = ss.str();

  {
    H5::H5File f(output_file.c_str(), H5F_ACC_RDWR);

    hdf5_tools::write_dataset(f, dataset_name, file_contents);
  }

  return 0;
}
