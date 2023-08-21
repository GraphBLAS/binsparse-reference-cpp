#pragma once

#include <cassert>

namespace binsparse {

template <typename T>
struct type_info;

template <typename T>
requires(std::is_const_v<T> || std::is_volatile_v<T>)
struct type_info<T> {
  static constexpr auto label() noexcept {
    return type_info<std::remove_cv_t<T>>::label();
  }
};

template <>
struct type_info<uint8_t> {
  static constexpr auto label() noexcept {
    return "uint8";
  }
};

template <>
struct type_info<uint16_t> {
  static constexpr auto label() noexcept {
    return "uint16";
  }
};

template <>
struct type_info<uint32_t> {
  static constexpr auto label() noexcept {
    return "uint32";
  }
};

template <>
struct type_info<uint64_t> {
  static constexpr auto label() noexcept {
    return "uint64";
  }
};

template <>
struct type_info<std::size_t> {
  static constexpr auto label() noexcept {
    return "uint64";
  }
};

template <>
struct type_info<int8_t> {
  static constexpr auto label() noexcept {
    return "int8";
  }
};

template <>
struct type_info<int16_t> {
  static constexpr auto label() noexcept {
    return "int16";
  }
};

template <>
struct type_info<int32_t> {
  static constexpr auto label() noexcept {
    return "int32";
  }
};

template <>
struct type_info<int64_t> {
  static constexpr auto label() noexcept {
    return "int64";
  }
};

template <>
struct type_info<float> {
  static constexpr auto label() noexcept {
    return "float32";
  }
};

template <>
struct type_info<double> {
  static constexpr auto label() noexcept {
    return "float64";
  }
};

template <>
struct type_info<bool> {
  static constexpr auto label() noexcept {
    return "bint8";
  }
};

} // end binsparse