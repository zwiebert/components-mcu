#include <osapi.h>

char *
itoa(int val, char *s, int radix) {
  if (s)
    os_sprintf(s, (radix == 10 ? "%i" : "%x"), val);
  return s;
}

char *
ltoa(long val, char *s, int radix) {
  if (s)
    os_sprintf(s, (radix == 10 ? "%li" : "%lx"), val);
  return s;

}
