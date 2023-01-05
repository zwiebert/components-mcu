#include "utils_misc/itoa.h"
#include "utils_misc/int_types.h"
#include <string.h>

char* ftoa(float f, char *buf, int n) {
  int i;
  int32_t mult;
  uint32_t rop;
  int16_t lop = (int16_t) f;
  char *s = buf;

  itoa(lop, s, 10);
  strcat(s, ".");
  s += strlen(s);

  f -= lop;

  mult = (lop < 0) ? -1 : 1;

  for (i = 0; i < n; ++i)
    mult *= 10;

  rop = (uint32_t) (f * mult + .5);
  ltoa(rop, s, 10);

  return buf;
}
