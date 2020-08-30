#include "misc/cstring_utils.hh"
#include <string.h>
#include <malloc.h>


char* csu_create(const char *src) {
  char *dst = static_cast<char *>(malloc(strlen(src)+1));
  if (dst)
    strcpy(dst, src);
  return dst;
}

bool csu_assign(char **dst, const char *src) {
  if (*dst && std::strcmp(*dst, src) == 0)
    return true;

  size_t src_size = std::strlen(src) + 1;

  if (src_size == 1) {
    free(*dst);
    *dst = 0;
    return true;
  }

  if (char *ptr = static_cast<char*>(realloc(*dst, src_size))) {
    *dst = std::strcpy(ptr, src);
    return true;
  }

  return false;
}

void csu_delete(char *p) {
  free(p);
}


void csu_destroy(char **p) {
  csu_delete(*p);
  *p = nullptr;
}



