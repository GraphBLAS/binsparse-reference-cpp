# Binsparse Reference Implementation

This library is a reference implementation of the [Binsparse Binary Sparse Format Specification](https://github.com/GraphBLAS/binsparse-specification) written using C++.

Binsparse is a cross-platform, embeddable format for storing sparse matrices
and other sparse data in an efficient binary format.  This library currently
only uses HDF5 as the underlying binary container format.

## C++ Binsparse Interface

This library provides a C++ interface for reading and writing Binsparse matrices.

```cpp
#include <binsparse/binsparse.hpp>
int main(int argc, char** argv) {
  /* Read in Binsparse matrix stored in binary COO format. */
  std::string file_name = "my_matrix.bsp.hdf5";
  auto mat = binsparse::read_coo_matrix<float, std::size_t>(file_name);

  /* Here we're*/

  binsparse::write_coo_matrix(file_name, mat);

  return 0;
}
```

## Binsparse Converter

There is also a program `convert_binsparse` in the `examples` directory that can
be used to convert from MatrixMarket format to Binsparse format.

```bash
# Convert from MatrixMarket to Binsparse binary format using COO.
bbrock@mymac:~/matrices$ ./convert_binsparse mat.mtx mat.bsp.hdf5 COO
```

## Building

This library uses CMake.  It should be able to automatically download and build
all dependencies except for:

- A recent compiler supporting C++20.
- libhdf5

It should be able to automatically detect the HDF5 installation on your system.

```bash
bbrock@mymac:~/src/binsparse-reference-impl$ cmake -B build
-- The C compiler identification is AppleClang 15.0.0.15000040
-- The CXX compiler identification is AppleClang 15.0.0.15000040
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /Library/Developer/CommandLineTools/usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /Library/Developer/CommandLineTools/usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
CMake Warning (dev) at /opt/homebrew/Cellar/cmake/3.27.7/share/cmake/Modules/FetchContent.cmake:1316 (message):
  The DOWNLOAD_EXTRACT_TIMESTAMP option was not given and policy CMP0135 is
  not set.  The policy's OLD behavior will be used.  When using a URL
  download, the timestamps of extracted files should preferably be that of
  the time of extraction, otherwise code that depends on the extracted
  contents might not be rebuilt if the URL changes.  The OLD behavior
  preserves the timestamps from the archive instead, but this is usually not
  what you want.  Update your project to the NEW behavior or specify the
  DOWNLOAD_EXTRACT_TIMESTAMP option with a value of true to avoid this
  robustness issue.
Call Stack (most recent call first):
  CMakeLists.txt:10 (FetchContent_Declare)
This warning is for project developers.  Use -Wno-dev to suppress it.

-- Found Python: /opt/homebrew/Frameworks/Python.framework/Versions/3.11/bin/python3.11 (found version "3.11.6") found components: Interpreter
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE
-- Version: 10.1.0
-- Build type:
-- Performing Test HAS_NULLPTR_WARNING
-- Performing Test HAS_NULLPTR_WARNING - Success
-- Found HDF5: /opt/homebrew/Cellar/hdf5/1.14.2/lib/libhdf5_cpp.dylib;/opt/homebrew/Cellar/hdf5/1.14.2/lib/libhdf5.dylib;/opt/homebrew/opt/libaec/lib/libsz.dylib;/Library/Developer/CommandLineTools/SDKs/MacOSX14.0.sdk/usr/lib/libz.tbd;/Library/Developer/CommandLineTools/SDKs/MacOSX14.0.sdk/usr/lib/libdl.tbd;/Library/Developer/CommandLineTools/SDKs/MacOSX14.0.sdk/usr/lib/libm.tbd (found version "1.14.2") found components: CXX
-- Configuring done (6.1s)
-- Generating done (0.1s)
-- Build files have been written to: /Users/bbrock/src/binsparse-reference-impl/build
bbrock@FVFJ21QKQ6LWmac:~/src/binsparse-reference-impl$ cd build/examples/
bbrock@FVFJ21QKQ6LWmac:~/src/binsparse-reference-impl/build/examples$ make -j
[ 11%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/format.cc.o
[ 22%] Building CXX object _deps/fmt-build/CMakeFiles/fmt.dir/src/os.cc.o
[ 33%] Linking CXX static library libfmt.a
[ 33%] Built target fmt
[ 55%] Building CXX object examples/CMakeFiles/test.dir/test.cpp.o
[ 55%] Building CXX object examples/CMakeFiles/convert_binsparse.dir/convert_binsparse.cpp.o
[ 66%] Building CXX object examples/CMakeFiles/inspect_binsparse.dir/inspect_binsparse.cpp.o
[ 77%] Linking CXX executable test
[ 77%] Built target test
[ 88%] Linking CXX executable inspect_binsparse
[ 88%] Built target inspect_binsparse
[100%] Linking CXX executable convert_binsparse
[100%] Built target convert_binsparse
bbrock@mymac:~/src/binsparse-reference-impl/build/examples$
```