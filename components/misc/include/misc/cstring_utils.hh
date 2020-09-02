#pragma once
#include <cstring>


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



