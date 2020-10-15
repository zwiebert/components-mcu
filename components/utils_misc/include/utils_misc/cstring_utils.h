#ifdef __cplusplus
#include "cstring_utils.hh"

  extern "C" {
#endif
#pragma once

  char* csu_create(const char *src);
  bool csu_assign(char **dst, const char *src);
  void csu_delete(char *p);
  void csu_destroy(char **p);
  inline const char* csu_get(char *p) {
    return p ? p : "";
  }
#ifdef __cplusplus
  }
#endif





