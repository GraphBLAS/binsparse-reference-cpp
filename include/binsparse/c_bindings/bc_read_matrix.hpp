#pragma once

#include <binsparse/binsparse.hpp>
#include <binsparse/c_bindings/allocator_wrapper.hpp>
#include <binsparse/c_bindings/binsparse_matrix.h>
#include <cstdio>

extern "C" {

bc_matrix_struct bc_read_matrix(const char* fname) {
  H5::H5File f(fname, H5F_ACC_RDWR);

  auto metadata = hdf5_tools::read_dataset<char>(f, "metadata");

  using json = nlohmann::json;
  auto data = json::parse(metadata);

  bc_matrix_struct matrix_struct;

  if (data["format"] == "COO") {

    auto value_type = hdf5_tools::dataset_type(f, "values");
    auto index_type = hdf5_tools::dataset_type(f, "indices_0");

    if (value_type == H5::PredType::IEEE_F32LE &&
        index_type == H5::PredType::STD_U64LE) {
      using T = float;
      using I = uint64_t;
      auto matrix = binsparse::read_coo_matrix<T, I>(fname);

      matrix_struct.value_type = bc_type_fp32;
      matrix_struct.pointer_type = bc_type_uint64;
      matrix_struct.index_type = bc_type_uint64;

      matrix_struct.rank = 2;
      matrix_struct.iso_valued = false;
      matrix_struct.type_size = sizeof(T);
      matrix_struct.values = matrix.values;
      matrix_struct.values_size = matrix.nnz * sizeof(T);
      matrix_struct.nvals = matrix.nnz;

      matrix_struct.axis = new bc_axis_struct[2];

      matrix_struct.axis[0].order = 0;
      matrix_struct.axis[0].dimension = matrix.m;
      matrix_struct.axis[0].in_order = true;
      matrix_struct.axis[0].index = matrix.rowind;
      matrix_struct.axis[0].nindex = matrix.nnz;
      matrix_struct.axis[0].index_size = matrix.nnz * sizeof(I);

      matrix_struct.axis[1].order = 0;
      matrix_struct.axis[1].dimension = matrix.n;
      matrix_struct.axis[1].in_order = true;
      matrix_struct.axis[1].index = matrix.colind;
      matrix_struct.axis[1].nindex = matrix.nnz;
      matrix_struct.axis[1].index_size = matrix.nnz * sizeof(I);

      return matrix_struct;
    }
  } else {
    fprintf(stderr, "Error: file format not supported.\n");
    assert(false);
  }
  assert(false);
}
}
