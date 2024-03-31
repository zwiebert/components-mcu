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



#ifdef CONFIG_APP_USE_HTTP
#include "net_http_server/http_server_setup.h"
inline struct cfg_http* config_read_httpServer(struct cfg_http *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_charArray(h, CB_HTTP_USER, c->user);
    kvsRead_charArray(h, CB_HTTP_PASSWD, c->password);
    kvsRead_i8(h, CB_HTTP_ENABLE, c->enable);

    kvs_close(h);
  }
  return c;
}
inline void config_setup_httpServer() {
  struct cfg_http c;
  config_read_httpServer(&c);
  hts_setup(&c);
}

inline bool config_read_http_enable() {
  return !!config_read_item(CB_HTTP_ENABLE, cfg_http().enable);
}
#endif



#include <uout/so_target_desc.hh>

#ifdef CONFIG_APP_USE_HTTP
#include "net_http_server/http_server_setup.h"
inline void soCfg_HTTP(class UoutWriter &td) {
  cfg_http c;
  config_read_httpServer(&c);

  td.so().print(settings_get_optKeyStr(CB_HTTP_USER), c.user);
  td.so().print(settings_get_optKeyStr(CB_HTTP_PASSWD), *c.password ? "*" : "");
  td.so().print(settings_get_optKeyStr(CB_HTTP_ENABLE), c.enable);
}

inline void soCfg_HTTP_ENABLE(class UoutWriter &td) {
  cfg_http c;
  td.so().print(settings_get_optKeyStr(CB_HTTP_ENABLE), config_read_httpServer(&c)->enable);
}
inline void soCfg_HTTP_USER(class UoutWriter &td) {
  cfg_http c;
  td.so().print(settings_get_optKeyStr(CB_HTTP_USER), config_read_httpServer(&c)->user);
}
inline void soCfg_HTTP_PASSWORD(class UoutWriter &td) {
  cfg_http c;
  td.so().print(settings_get_optKeyStr(CB_HTTP_PASSWD), *config_read_httpServer(&c)->password ? "*" : "");
}
#endif


template<typename settings_type>
constexpr void net_http_server_register_settings(settings_type &settings){
#ifdef CONFIG_APP_USE_HTTP
    settings.initField(CB_HTTP_ENABLE, "C_HTTP_ENABLE", otok::k_http_enable, CBT_i8, soCfg_HTTP_ENABLE, STF_direct_bool);
    settings.initField(CB_HTTP_USER, "C_HTTP_USER", otok::k_http_user, CBT_str, soCfg_HTTP_USER, STF_direct);
    settings.initField(CB_HTTP_PASSWD, "C_HTTP_PASSWD", otok::k_http_password, CBT_str, soCfg_HTTP_PASSWORD, STF_direct);
#endif
}
#endif
