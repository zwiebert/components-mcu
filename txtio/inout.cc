
#include "txtio/inout.h"
#include "txtio/txtio_setup.hh"
#include "txtio_app_cfg.h"
#include "txtio_imp.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "utils_misc/int_macros.h"
#include "utils_misc/itoa.h"
#include "txtio/txtio_mutex.hh"


enum verbosity txtio_verbose;

int (*io_putc_fun)(char c);
int (*io_getc_fun)(void);
int (*con_printf_fun)(const char *fmt, ...);

int io_putc(char c) {
  int result = -1;

  if (io_putc_fun) {
    { LockGuard lock(txtio_mutex);
      result = io_putc_fun(c);
    }
  }
  return result;
}

int io_getc(void) {
  int result = -1;

  if (io_getc_fun) {
    { LockGuard lock(txtio_mutex);
      result = io_getc_fun();
    }
  }
  return result;
}


#if 0
/* use io_putc()/io_getc(). Don't use the function pointers for
   putc/get directly. */
#define io_putc_fun #error
#define io_getc_fun #error
#endif

int  io_putlf(void) { return io_putc('\n'); }


int io_puts(const char *s) {
  int result = 0;
  { LockGuard lock(txtio_mutex);
    if (!io_putc_fun)
      return -1;

    for (; *s != '\0'; ++s) {
      if (io_putc_fun(*s) == -1) {
        return -1;
        ++result;
      }
    }
  }
  return result;
}


int io_write(const char *s, unsigned len) {
  int result = len;
  { LockGuard lock(txtio_mutex);
    if (!io_putc_fun)
      return -1;

    for (; len > 0; ++s, --len) {
      if (io_putc_fun(*s) == -1) {
        result = -1;
        break;
      }
    }
  }
  return result - len;
}

void 
io_putx8(uint8_t n) {
  char s[3];
  if (n & 0xf0) {
    itoa(n, s, 16);
  } else {
    s[0] = '0';
    itoa(n, s+1, 16);
  }
  io_puts(s);
}



void 
io_print_hex_8(uint8_t n, bool comma) {
  char s[3];
  itoa(n, s, 16);
  io_puts((n & 0xF0) ? "0x" : "0x0");
  io_puts(s);
  if (comma)
    io_puts(", ");
}


void 
io_print_hex_16(uint16_t n, bool comma) {
  char s[5];
  itoa(n, s, 16);
  io_puts((n & 0xFFF0)== 0 ? "0x000" :
          (n & 0xFF00)== 0 ? "0x00" :
          (n & 0xF000)== 0 ? "0x0" : "0x");

  io_puts(s);

  if (comma)
    io_puts(", ");
}

void 
io_print_hex_32(uint32_t n, bool comma) {
  char s[10];
  ltoa(n, s, 16);
  io_puts((n & 0xFFFFFFF0)== 0 ? "0x0000000" :
          (n & 0xFFFFFF00)== 0 ? "0x000000" :
          (n & 0xFFFFF000)== 0 ? "0x00000" :
          (n & 0xFFFF0000)== 0 ? "0x0000" :
          (n & 0xFFF00000)== 0 ? "0x000" :
          (n & 0xFF000000)== 0 ? "0x00" :
          (n & 0xF0000000)== 0 ? "0x0" :
          "0x");

  io_puts(s);

  if (comma)
    io_puts(", ");
}

void 
io_print_hex(uint32_t n, bool prefix) {
  char s[10];
  ltoa(n, s, 16);
  if (prefix)
    io_puts("0x");
  io_puts(s);
}

void 
io_print_dec_32(int32_t n, bool comma) {
  char s[12];
  ltoa(n, s, 10);
  io_puts(s);

  if (comma)
    io_puts(", ");
}

void 
io_print_dec_16(int16_t n, bool comma) {
  char s[10];
  io_puts(itoa(n, s, 10));
  if (comma)
    io_puts(", ");
}

void  io_print_float(float f, int n) {
	int i;
	int32_t mult;
	uint32_t rop;
	int16_t lop = (int16_t) f;
	io_print_dec_16(lop, false);
	io_putc('.');

	f -= lop;

	mult = (lop < 0) ? -1 : 1;

	for (i = 0; i < n; ++i)
		mult *= 10;

	rop = (uint32_t) (f * mult);

	io_print_dec_32(rop, false);
}




void 
io_putn(int n, int radix) {
  char s[10];
  io_puts(itoa(n, s, radix));
}


void 
io_putl(int32_t n, int radix) {
  char s[13];
  io_puts(ltoa(n, s, radix));
}


void 
io_putd(int n) {
  io_putn(n, 10);
}


void 
io_putld(int32_t n) {
  io_putl(n, 10);
}

int io_getline(char *buf, unsigned buf_size) {
  int i, c;

  { LockGuard lock(txtio_mutex);
    if (!io_getc_fun)
      return -1;

    for (i = 0; (i + 1) < buf_size; ++i) {
      c = io_getc_fun();

      if (c == -1)
        return -1;

      if (c == ';')
        break;

      buf[i] = (char) c;
    }

    buf[i] = '\0';
    return i;
  }
  return -1;
}

void 
printBCD(uint8_t bcd) {
  char s[10];
  io_puts(itoa(GET_HIGH_NIBBLE(bcd), s , 16)); 
  io_puts(itoa(GET_LOW_NIBBLE( bcd), s , 16)); 
}

void 
print_array_8(const uint8_t *src, int len) {
  int i;

  for (i = 0; i < len; ++i) {
    io_print_hex_8(src[i], true);
  }
  io_putlf();
}

void 
print_array_8_inv(const uint8_t *src, int len) {
  int i;

  for (i = 0; i < len; ++i) {
    io_print_hex_8(~src[i], true);
  }
  io_putlf();
}



void txtio_setup(struct cfg_txtio *cfg_txtio) {
  if (cfg_txtio) {
    txtio_verbose = cfg_txtio->verbose;
  }
  txtio_mcu_setup(cfg_txtio);
}
