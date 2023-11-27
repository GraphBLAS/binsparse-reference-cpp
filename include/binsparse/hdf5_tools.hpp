#pragma once

#include <H5Cpp.h>
#include <cassert>
#include <ranges>
#include <vector>

#include <iostream>

namespace hdf5_tools {

template <typename U>
inline H5::PredType get_hdf5_native_type() {
  using T = std::decay_t<U>;
  if constexpr (std::is_same_v<T, char>) {
    return H5::PredType::NATIVE_CHAR;
  } else if constexpr (std::is_same_v<T, unsigned char>) {
    return H5::PredType::NATIVE_UCHAR;
  } else if constexpr (std::is_same_v<T, short>) {
    return H5::PredType::NATIVE_SHORT;
  } else if constexpr (std::is_same_v<T, unsigned short>) {
    return H5::PredType::NATIVE_USHORT;
  } else if constexpr (std::is_same_v<T, int>) {
    return H5::PredType::NATIVE_INT;
  } else if constexpr (std::is_same_v<T, unsigned int>) {
    return H5::PredType::NATIVE_UINT;
  } else if constexpr (std::is_same_v<T, long>) {
    return H5::PredType::NATIVE_LONG;
  } else if constexpr (std::is_same_v<T, unsigned long>) {
    return H5::PredType::NATIVE_ULONG;
  } else if constexpr (std::is_same_v<T, long long>) {
    return H5::PredType::NATIVE_LLONG;
  } else if constexpr (std::is_same_v<T, unsigned long long>) {
    return H5::PredType::NATIVE_ULLONG;
  } else if constexpr (std::is_same_v<T, float>) {
    return H5::PredType::NATIVE_FLOAT;
  } else if constexpr (std::is_same_v<T, double>) {
    return H5::PredType::NATIVE_DOUBLE;
  } else if constexpr (std::is_same_v<T, long double>) {
    return H5::PredType::NATIVE_LDOUBLE;
  } else {
    assert(false);
  }
}

template <typename U>
inline H5::PredType get_hdf5_standard_type() {
  using T = std::decay_t<U>;
  if constexpr (std::is_same_v<T, char>) {
    return H5::PredType::STD_I8LE;
  } else if constexpr (std::is_same_v<T, unsigned char>) {
    return H5::PredType::STD_U8LE;
  } else if constexpr (std::is_same_v<T, int8_t>) {
    return H5::PredType::STD_I8LE;
  } else if constexpr (std::is_same_v<T, uint8_t>) {
    return H5::PredType::STD_U8LE;
  } else if constexpr (std::is_same_v<T, int16_t>) {
    return H5::PredType::STD_I16LE;
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    return H5::PredType::STD_U16LE;
  } else if constexpr (std::is_same_v<T, int32_t>) {
    return H5::PredType::STD_I32LE;
  } else if constexpr (std::is_same_v<T, uint32_t>) {
    return H5::PredType::STD_U32LE;
  } else if constexpr (std::is_same_v<T, int64_t>) {
    return H5::PredType::STD_I64LE;
  } else if constexpr (std::is_same_v<T, uint64_t>) {
    return H5::PredType::STD_U64LE;
  } else if constexpr (std::is_same_v<T, std::size_t>) {
    return H5::PredType::STD_U64LE;
  } else if constexpr (std::is_same_v<T, float>) {
    return H5::PredType::IEEE_F32LE;
  } else if constexpr (std::is_same_v<T, double>) {
    return H5::PredType::IEEE_F64LE;
  } else {
    assert(false);
  }
}

inline H5::PredType get_type(H5::DataSet& dataset) {
  H5T_class_t type_class = dataset.getTypeClass();

  if (type_class == H5T_INTEGER) {
    H5::IntType intype = dataset.getIntType();

    H5std_string order_string;
    H5T_order_t order = intype.getOrder(order_string);

    assert(order == H5T_ORDER_LE);

    size_t size = intype.getSize();

    if (intype.getSign() == H5T_SGN_NONE && size == sizeof(std::uint64_t)) {
      return H5::PredType::STD_U64LE;
    } else if (intype.getSign() == H5T_SGN_2 && size == sizeof(std::int64_t)) {
      return H5::PredType::STD_I64LE;
    } else {
      assert(false);
    }
  } else if (type_class == H5T_FLOAT) {
    H5::FloatType floatype = dataset.getFloatType();

    H5std_string order_string;
    H5T_order_t order = floatype.getOrder(order_string);

    assert(order == H5T_ORDER_LE);

    size_t size = floatype.getSize();

    if (size == sizeof(float)) {
      return H5::PredType::IEEE_F32LE;
    } else if (size == sizeof(double)) {
      return H5::PredType::IEEE_F64LE;
    } else {
      assert(false);
    }
  } else {
    assert(false);
  }
}

template <typename H5GroupOrFile, std::ranges::contiguous_range R>
void write_dataset(H5GroupOrFile& f, const std::string& label, R&& r) {
  using T = std::ranges::range_value_t<R>;
  hsize_t size = std::ranges::size(r);
  H5::DataSpace dataspace(1, &size);
  auto dataset =
      f.createDataSet(label.c_str(), get_hdf5_standard_type<T>(), dataspace);

  dataset.write(std::ranges::data(r), get_hdf5_native_type<T>());
  dataset.close();
  dataspace.close();
}

inline std::string get_attribute(H5::H5Object& f, const std::string& key) {
  auto attribute = f.openAttribute(key.c_str());

  H5::DataType type = attribute.getDataType();

  auto size = type.getSize();

  std::string attribute_string(" ", size);

  attribute.read(type, attribute_string.data());

  attribute.close();

  return attribute_string;
}

inline void set_attribute(H5::H5Object& f, const std::string& key,
                          const std::string& value) {
  H5::StrType string_type(H5::PredType::C_S1, value.size());
  hsize_t size = value.size();
  H5::DataSpace dataspace(1, &size);

  auto attribute = f.createAttribute(key.c_str(), string_type, H5S_SCALAR);

  attribute.write(string_type, value.c_str());
  attribute.close();
}

template <typename T, typename Allocator, typename H5GroupOrFile>
std::span<T> read_dataset(H5GroupOrFile& f, const std::string& label,
                          Allocator&& alloc) {
  H5::DataSet dataset = f.openDataSet(label.c_str());

  H5::DataSpace space = dataset.getSpace();
  hsize_t ndims = space.getSimpleExtentNdims();
  assert(ndims == 1);
  hsize_t dims;
  space.getSimpleExtentDims(&dims, &ndims);
  space.close();

  T* data = alloc.allocate(dims);
  dataset.read(data, get_hdf5_native_type<T>());
  dataset.close();
  return std::span<T>(data, dims);
}

template <typename T, typename H5GroupOrFile>
std::span<T> read_dataset(H5GroupOrFile& f, const std::string& label) {
  return read_dataset<T>(f, label, std::allocator<T>{});
}

template <typename H5GroupOrFile>
inline H5::PredType dataset_type(H5GroupOrFile& f, const std::string& label) {
  H5::DataSet dataset = f.openDataSet(label.c_str());
  auto type = get_type(dataset);
  dataset.close();
  return type;
}

} // namespace hdf5_tools
