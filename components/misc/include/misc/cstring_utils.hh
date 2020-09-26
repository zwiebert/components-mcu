#pragma once
#include <cstring>


#pragma GCC push_options
#pragma GCC optimize ("O0")
//////////////////////// safe copy ////////////////////////////////////////////////
extern "C++" {
#include <type_traits>
extern void (*strcpy_error_handler)();

template<typename T>
inline typename std::enable_if<std::is_convertible<T, char*>::value, char*>::type csu_copy(T const &dst, const char *src) {
  return std::strcpy(dst, src);
}

template<typename T>
inline typename std::enable_if<std::is_convertible<T, char*>::value, char*>::type
csu_copy(T const &dst, const size_t dst_size, const char *src, const size_t src_len) {
  if (src_len >= dst_size)
    strcpy_error_handler();

  if (src_len >= strlen(src))
    std::strcpy(dst, src);
  else {
    std::memcpy(dst, src, src_len);
    dst[src_len] = '\0';
  }
  return dst;
}

template<typename T>
inline typename std::enable_if<std::is_convertible<T, char*>::value, char*>::type csu_copy(T const&dst, const size_t dst_size, const char *src) {
  const size_t src_len = std::strlen(src);
  if (src_len >= dst_size)
    strcpy_error_handler();
  std::memcpy(dst, src, src_len + 1);
  return dst;
}

template<int dst_size>
inline char* csu_copy(char (&dst)[dst_size], const char *src) {
  char *dst_ptr = dst;
  return csu_copy(dst_ptr, dst_size, src);
}

template<int dst_size>
inline char* csu_copy(char (&dst)[dst_size], size_t /*dst_size*/, const char *src) {
  char *dst_ptr = dst;
  return csu_copy(dst_ptr, dst_size, src);
}

template<int dst_size>
inline char* csu_copy(char (&dst)[dst_size], size_t /*dst_size*/, const char *src, const size_t src_len) {
  char *dst_ptr = dst;
  return csu_copy(dst_ptr, dst_size, src, src_len);
}



/////////////////////// copy_cat ///////////////////////


inline int csu_copy_cat_unsafe(char *const &dst, const char *src1) {
  const size_t src1_len = strlen(src1);
  memcpy(dst, src1, src1_len + 1);
  return src1_len;
}
template<typename ... cchar_ptrT>
inline int csu_copy_cat_unsafe(char *const &dst_ptr, const char *src1, cchar_ptrT ...args) {
  int src1_len = csu_copy_cat_unsafe(dst_ptr, src1);
  return src1_len + csu_copy_cat_unsafe(dst_ptr + src1_len, args...);
}

inline int csu_copy_cat(char * const &dst, const size_t dst_len, const char *src1) {
  const size_t src1_len = strlen(src1);

  if (src1_len  > dst_len)
    strcpy_error_handler();

  memcpy(dst, src1, src1_len + 1);
  return src1_len;
}

typedef const char *cchar_ptrT;

template<typename ... cchar_ptrT>
inline int csu_copy_cat(char * const &dst_ptr, const size_t dst_len, const char *src1, cchar_ptrT ...args) {
  int src1_len = csu_copy_cat(dst_ptr, dst_len, src1);
  return src1_len + csu_copy_cat(dst_ptr + src1_len, dst_len - src1_len, args...);
}

template<const size_t dst_size,  typename ... cchar_ptrT>
inline int csu_copy_cat(char (&dst)[dst_size], const char *src1, cchar_ptrT ...args) {
  char *dst_ptr = dst;
  const size_t dst_len = dst_size - 1;

  return csu_copy_cat(dst_ptr, dst_len, src1, args...);
}

/////////////////////////////////////////////////////////////////////////////////
#pragma GCC pop_options

struct HasRealloc {
   enum { hasRealloc = 1 };
};

struct HasNotRealloc {
   enum { hasRealloc = 0 };
};

extern char csu_empty_[1];

template<class Allocator, class HR = HasNotRealloc>
class csut {
  static char mEmpty[1];
  char *mStr = 0;
protected:

  void csu_delete(char *p) {
    Allocator().deallocate(p, 0);
  }

  void csu_destroy(char **p) {
    csu_delete(*p);
    *p = nullptr;
  }


  bool csu_assign(char **dst, const char *src) {
    if (*dst && std::strcmp(*dst, src) == 0)
      return true;

    size_t src_size = std::strlen(src) + 1;

    if (src_size == 1) {
      csu_destroy(dst);
      return true;
    }

    if constexpr (HR::hasRealloc) {
      if (char *ptr = Allocator().reallocate(*dst, src_size)) {
        *dst = std::strcpy(ptr, src);
        return true;
      }
    } else {
      csu_destroy(dst);
      if ((*dst = Allocator().allocate(src_size))) {
        std::strcpy(*dst, src);
        return true;
      }
    }

    return false;
  }
public:

  bool operator==(csut &rhs) {
    if (mStr == nullptr || rhs.mStr == nullptr)
      return mStr == rhs.mStr;
    else
      return std::strcmp(mStr, rhs.mStr) == 0;
  }

public:
  csut() {
  }
  ~csut() {
    csu_delete(mStr);
  }
  explicit csut(const char *s) {
    csu_assign(&mStr, s);
  }
  explicit csut(const char *s, size_t src_len) {
    mStr = Allocator().allocate(src_len + 1);
    memcpy(mStr, s, src_len);
    mStr[src_len] = '\0';
  }

  csut(csut &other) {
    csu_assign(&mStr, other.mStr);
  }
  csut(csut &&other) noexcept {
    mStr = other.mStr;
    other.mStr = 0;
  }
  csut& operator=(const char *rhs) {
    csu_assign(&mStr, rhs);
    return *this;
  }
  csut& operator=(csut &&rhs) noexcept {
    if (this != &rhs) {
      mStr = rhs.mStr;
      rhs.mStr = 0;
    }
    return *this;
  }

  const char *ptr() const {
    return mStr ? mStr : "";
  }

  char *ptr() {
    return mStr ? mStr : csu_empty_;
  }

  operator const char *() const {
    return mStr ? mStr : "";
  }
  operator char *() {
    return mStr ? mStr : csu_empty_;
  }
  //bool operator==(csut &rhs) const;
  bool operator!=(csut &rhs) const {
    return !(*this == rhs);
  }
};


#include "allocator_malloc.hh"
using csu = csut<AllocatorMalloc<char>, HasRealloc>;
static_assert(sizeof(csu) == sizeof(char *));

}



