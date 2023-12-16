#pragma once

#include <cassert>
#include <functional>
#include <type_traits>

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

namespace __detail {

template <typename Fn, typename... Args>
  requires(std::is_invocable_v<Fn, Args...>)
void invoke_if_able(Fn&& fn, Args&&... args) {
  std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
}

template <typename Fn, typename... Args>
void invoke_if_able(Fn&& fn, Args&&... args) {}

template <typename Fn, typename... Args>
void invoke_visit_fn_impl_(std::vector<std::string> type_labels, Fn&& fn,
                           Args&&... args) {
  if constexpr (sizeof...(Args) <= 3) {
    // The first label we consume is the value label
    if (type_labels.size() == 3) {
      auto type_label = type_labels.front();
      if (type_label == "uint8") {
        invoke_if_able(std::forward<Fn>(fn), std::uint8_t(),
                       std::forward<Args>(args)...);
      } else if (type_label == "uint16") {
        invoke_if_able(std::forward<Fn>(fn), std::uint16_t(),
                       std::forward<Args>(args)...);
      } else if (type_label == "uint32") {
        invoke_if_able(std::forward<Fn>(fn), std::uint32_t(),
                       std::forward<Args>(args)...);
      } else if (type_label == "uint64") {
        invoke_if_able(std::forward<Fn>(fn), std::uint64_t(),
                       std::forward<Args>(args)...);
      } else if (type_label == "int8") {
        invoke_if_able(std::forward<Fn>(fn), std::int8_t(),
                       std::forward<Args>(args)...);
      } else if (type_label == "int16") {
        invoke_if_able(std::forward<Fn>(fn), std::int16_t(),
                       std::forward<Args>(args)...);
      } else if (type_label == "int32") {
        invoke_if_able(std::forward<Fn>(fn), std::int32_t(),
                       std::forward<Args>(args)...);
      } else if (type_label == "int64") {
        invoke_if_able(std::forward<Fn>(fn), std::int64_t(),
                       std::forward<Args>(args)...);
      } else if (type_label == "float32") {
        invoke_if_able(std::forward<Fn>(fn), float(),
                       std::forward<Args>(args)...);
      } else if (type_label == "float64") {
        invoke_if_able(std::forward<Fn>(fn), double(),
                       std::forward<Args>(args)...);
      } else if (type_label == "bint8") {
        invoke_if_able(std::forward<Fn>(fn), bool(),
                       std::forward<Args>(args)...);
      } else {
        assert(false);
      }
    } else {
      // The next two types are index types, which must be integrals.
      auto type_label = type_labels.back();
      type_labels.pop_back();
      if (type_label == "uint8") {
        invoke_visit_fn_impl_(type_labels, std::forward<Fn>(fn), std::uint8_t(),
                              std::forward<Args>(args)...);
      } else if (type_label == "uint16") {
        invoke_visit_fn_impl_(type_labels, std::forward<Fn>(fn),
                              std::uint16_t(), std::forward<Args>(args)...);
      } else if (type_label == "uint32") {
        invoke_visit_fn_impl_(type_labels, std::forward<Fn>(fn),
                              std::uint32_t(), std::forward<Args>(args)...);
      } else if (type_label == "uint64") {
        invoke_visit_fn_impl_(type_labels, std::forward<Fn>(fn),
                              std::uint64_t(), std::forward<Args>(args)...);
      } else if (type_label == "int8") {
        invoke_visit_fn_impl_(type_labels, std::forward<Fn>(fn), std::int8_t(),
                              std::forward<Args>(args)...);
      } else if (type_label == "int16") {
        invoke_visit_fn_impl_(type_labels, std::forward<Fn>(fn), std::int16_t(),
                              std::forward<Args>(args)...);
      } else if (type_label == "int32") {
        invoke_visit_fn_impl_(type_labels, std::forward<Fn>(fn), std::int32_t(),
                              std::forward<Args>(args)...);
      } else if (type_label == "int64") {
        invoke_visit_fn_impl_(type_labels, std::forward<Fn>(fn), std::int64_t(),
                              std::forward<Args>(args)...);
      } else {
        assert(false);
      }
    }
  }
}

} // namespace __detail

template <typename Fn>
inline void visit_label(const std::vector<std::string>& type_labels, Fn&& fn) {
  __detail::invoke_visit_fn_impl_(type_labels, fn);
}

} // namespace binsparse
