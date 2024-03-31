/**
 * \brief  optional header only glue inline functions, to glue components together.
 *         This header is supposed  to be included by an application.
 *
 *  To access this file, the caller need to have required modules: uout and config_kvs
 *  
 *
 */

#pragma once

#if __has_include(<config_kvs/settings.hh>) &&  __has_include(<uout/so_target_desc.hh>)
#include <config_kvs/settings.hh>
#include <config_kvs/comp_settings.hh>
#include <config_kvs/config_kvs.h>
#include <app_config/options.hh>
#include "txtio/inout.h"
#include "txtio/txtio_setup.hh"
#include <uout/uo_callbacks.h>
#include <uout/so_target_desc.hh>

inline struct cfg_txtio* config_read_txtio(struct cfg_txtio *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    int8_t verb = c->verbose;
    kvsRead_i8(h, CB_VERBOSE, verb);
    c->verbose = static_cast<verbosity>(verb);
    kvs_close(h);
  }
  return c;
}
inline void config_setup_txtio(struct uo_flagsT *flagsPtr) {
  uo_flagsT flags = *flagsPtr;
  flags.evt.pin_change = true;
  flags.evt.gen_app_state_change = true;
  flags.fmt.json = true;
  flags.fmt.txt = true;

  struct cfg_txtio c { .flags = flags, };
  config_read_txtio(&c);
  txtio_setup(&c);
}
inline enum verbosity config_read_verbose() {
  return static_cast<verbosity>(config_read_item(CB_VERBOSE, 3));
}


inline void soCfg_VERBOSE(class UoutWriter &td) {
  td.so().print(settings_get_optKeyStr(CB_VERBOSE), config_read_verbose());
}

template<typename settings_type>
constexpr void txtio_register_settings(settings_type &settings){
    settings.initField(CB_VERBOSE, "C_VERBOSE", otok::k_verbose, CBT_i8, soCfg_VERBOSE, STF_direct);
}
#endif
