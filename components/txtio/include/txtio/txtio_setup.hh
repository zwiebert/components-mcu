#pragma once
#include "inout.h"
#include "uout/uo_callbacks.h"

struct cfg_txtio {
  uo_flagsT flags; ///< additional flags for callback
  enum verbosity verbose;
  int baud;
};

extern struct cfg_txtio *txtio_config;
void txtio_setup(struct cfg_txtio *cfg_txtio);
