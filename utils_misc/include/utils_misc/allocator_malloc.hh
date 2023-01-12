#pragma once

#include <list>
#include <malloc.h>

template<class T>
class AllocatorMalloc {
public:
  typedef T value_type;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef T &reference;
  typedef const T &const_reference;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef std::true_type is_always_equal;
  typedef std::true_type propagate_on_container_move_assignment;


  constexpr AllocatorMalloc() noexcept {
  }

  constexpr AllocatorMalloc(const AllocatorMalloc &other) noexcept {
  }

  constexpr T* allocate(size_type n) {
    return static_cast<T*>(malloc(n * sizeof(T)));
  }

  constexpr T* reallocate(T *old, size_type n) {
    T *ptr = static_cast<T*>(realloc(static_cast<void*>(old), n * sizeof(T)));
    if (!ptr)
      deallocate(old, 0);
    return ptr;
  }

  constexpr void deallocate(T *p, size_type) {
    free(static_cast<void*>(p));
  }

  AllocatorMalloc<T>& operator=(const AllocatorMalloc&) {
    return *this;
  }

  template<class U>
  constexpr AllocatorMalloc(const AllocatorMalloc<U> &other) noexcept {
  }

  template<class U>
  AllocatorMalloc& operator=(const AllocatorMalloc<U>&) {
    return *this;
  }

};

template<class T1, class T2>
constexpr bool operator==(const AllocatorMalloc<T1> &lhs, const AllocatorMalloc<T2> &rhs) noexcept {
  return true;
}

//template<class T> using TList = std::list<T, AllocatorMalloc<T>>;
//#include <string>
//typedef std::basic_string<char, std::char_traits<char>, AllocatorMalloc<char>> String;
