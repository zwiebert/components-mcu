#pragma once
#include "inout.h"
#include "uout/uo_callbacks.h"

struct cfg_txtio {
  uo_flagsT flags; ///< additional flags for callback
  enum verbosity verbose;
  int baud = CONFIG_APP_UART_BAUD_RATE;
};

extern struct cfg_txtio *txtio_config;
void txtio_setup(struct cfg_txtio *cfg_txtio);
