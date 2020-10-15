#include "utils_misc/cstring_utils.h"
#include "utils_misc/cstring_utils.hh"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>


 char csu_empty_[1];

char* csu_create(const char *src) {
  char *dst = static_cast<char *>(malloc(strlen(src)+1));
  if (dst)
    strcpy(dst, src);
  return dst;
}

bool csu_assign(char **dst, const char *src) {
  if (*dst && strcmp(*dst, src) == 0)
    return true;

  size_t src_size = strlen(src) + 1;

  if (src_size == 1) {
    free(*dst);
    *dst = 0;
    return true;
  }

  if (char *ptr = static_cast<char*>(realloc(*dst, src_size))) {
    *dst = strcpy(ptr, src);
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


void (*strcpy_error_handler)() = abort;

