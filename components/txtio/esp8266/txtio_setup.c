#include "txtio/inout.h"


int ets_uart_printf(const char *fmt, ...);
int es_io_putc(char c);
int  es_io_getc(void);

void txtio_mcu_setup() {

  io_putc_fun = es_io_putc;
  io_getc_fun = es_io_getc;
  io_printf_fun = ets_uart_printf;
}
