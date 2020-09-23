#ifdef __cplusplus
  extern "C++" {
#endif
#pragma once

#include "stdbool.h"
#include "stdint.h"

enum verbosity {
  vrbNone, vrb1, vrb2, vrb3, vrb4, vrbDebug
};


struct cfg_txtio {
  enum verbosity verbose;
  int baud;
};
extern struct cfg_txtio *txtio_config;
#define TXTIO_IS_VERBOSE(lvl) (txtio_config->verbose >= (lvl))

void txtio_setup(struct cfg_txtio *cfg_txtio);


extern int (*io_putc_fun)(char c);
extern int (*io_getc_fun)(void);
extern int (*con_printf_fun)(const char *fmt, ...);
extern int (*io_puts_fun)(const char *s);

int io_putc(char c);
int io_getc(void);
int io_putlf(void);
int io_puts(const char *s);
int io_write(const char *s, unsigned len);
int io_getline(char *buf, unsigned buf_size);
int io_printf(const char *fmt, ...);

void io_print_hex_8(uint8_t n, bool comma);
void io_print_hex_16(uint16_t n, bool comma);
void io_print_hex_32(uint32_t n, bool comma);
void io_print_hex(uint32_t n, bool prefix);

void io_print_dec_8(int8_t n, bool comma);
void io_print_dec_16(int16_t n, bool comma);
void io_print_dec_32(int32_t n, bool comma);

void io_print_float(float f, int n);

void io_putn(int n, int radix);
void io_putl(int32_t n, int radix);
void io_putd(int n);
void io_putld(int32_t n);
void io_putx8(uint8_t n);

void printBCD(uint8_t bcd);
void print_array_8(const uint8_t *src, int len);
void print_array_8_inv(const uint8_t *src, int len);

#ifdef TEST_HOST
#include <stdio.h>
#define io_printf printf
#define con_printf printf
#elif defined MCU_ESP8266
#include <osapi.h>
#define io_printf(...)  do { char __buf[120]; if (ets_snprintf(__buf, sizeof __buf, __VA_ARGS__) > 0) io_puts(__buf); } while(0)
#define  con_printf (*con_printf_fun)
#else
#include <stdio.h>
#define io_printf(...)  do { char __buf[120]; if (snprintf(__buf, sizeof __buf, __VA_ARGS__) > 0) io_puts(__buf); } while(0)
#define  con_printf (*con_printf_fun)
#endif



#define io_printf_v(v, ...)   do { if(TXTIO_IS_VERBOSE(v)) io_printf(__VA_ARGS__); } while(0)
#define con_printf_v(v, ...)   do { if(TXTIO_IS_VERBOSE(v)) con_printf(__VA_ARGS__); } while(0)






#ifdef __cplusplus
  }
#endif
