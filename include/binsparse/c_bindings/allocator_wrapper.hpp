#pragma once

namespace binsparse {

template <typename T>
class allocator_wrapper {
public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  template <typename U>
  allocator_wrapper(const allocator_wrapper<U> &other) noexcept
      : malloc_fn_(other.malloc_fn_), free_fn_(other.free_fn_) {}

  allocator_wrapper(void* (*malloc_fn)(size_t), void (*free_fn)(void*)) noexcept
      : malloc_fn_(malloc_fn), free_fn_(free_fn) {}

  allocator_wrapper(const allocator_wrapper &) = default;
  allocator_wrapper &operator=(const allocator_wrapper &) = default;
  ~allocator_wrapper() = default;

  using is_always_equal = std::false_type;

  pointer allocate(std::size_t size) {
    return reinterpret_cast<T*>(malloc_fn_(size*sizeof(T)));
  }

  void deallocate(pointer ptr, std::size_t n) {
    free_fn_(ptr);
  }

  bool operator==(const allocator_wrapper &) const = default;
  bool operator!=(const allocator_wrapper &) const = default;

  template <typename U> struct rebind {
    using other = allocator_wrapper<U>;
  };


  void* (*malloc_fn_)(size_t);
  void  (*free_fn_)(void*);
};


}