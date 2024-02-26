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

inline bool csu_endsWith(const char *hay, const char *needle) {
  if (!*hay || !*needle)
    return false;

  if (auto p = strstr(hay, needle)) {
    const unsigned p_diff = p - hay;
    const unsigned l_diff = strlen(hay) - strlen(needle);
    return (p_diff == l_diff);
  }

  return false;
}

inline bool csu_startsWith(const char *hay, const char *needle) {
  if (!*hay || !*needle)
    return false;

  if (auto p = strstr(hay, needle)) {
    if (p == hay)
      return true;
  }

  return false;
}

inline bool csu_areEqual(const char *a, const char *b) {
  if (!*a || !*b)
    return false;

  return strcmp(a, b) == 0;
}

#ifdef __cplusplus
}
#endif

