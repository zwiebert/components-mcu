#include "../txtio_imp.h"
#include <txtio/inout.h>
#include <txtio/txtio_setup.hh>

#include <unistd.h>
#include <stdio.h>

#if 0

int io_puts(const char *s) {
  return fputs(s, stdout);
}

#endif


void txtio_mcu_setup(struct cfg_txtio *cfg_txtio) {
  io_putc_fun = [](char c) { return putchar(c); };
  io_getc_fun = []() { return getchar(); };
  con_printf_fun = printf;

}
