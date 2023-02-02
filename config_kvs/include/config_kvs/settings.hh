/**
 * \file config_kvs/settings.hh
 */

#pragma once

#include <app_config/options.hh>
#include <uout/so_target_desc.hh>
#include "config.h"
#include <assert.h>

enum KvsType : uint8_t {
  CBT_none, CBT_i8, CBT_u8, CBT_i16, CBT_u16, CBT_i32, CBT_u32, CBT_i64, CBT_u64, CBT_str, CBT_f, CBT_blob, CBT_end
};

enum StoreFun : uint8_t {
  STF_none, STF_direct, STF_direct_hex, STF_direct_bool
};

template<typename CfgItem>
class SettingsBase {
public:
  using soCfgFunT = void (*)(const UoutWriter &td);
  using storeFunOkT = void (*)(otok key, const char *val);
public:
  virtual const char* get_kvsKey(const CfgItem item) const = 0;
  virtual KvsType get_kvsType(const CfgItem item) const = 0;

  virtual otok get_optKey(const CfgItem item) const = 0;
  virtual const char* get_optKeyStr(const CfgItem item) const = 0;

  virtual soCfgFunT get_soCfgFun(const CfgItem item) const = 0;
  virtual soCfgFunT get_soCfgFun(const otok optKey) const = 0;

  virtual StoreFun get_storeFun(const CfgItem item) const = 0;
  virtual StoreFun get_storeFun(const otok optKey) const = 0;

  virtual CfgItem get_item(const otok optKey) const = 0;
};


template<typename Kvs_Type, typename CfgItem>
constexpr Kvs_Type config_read_item(CfgItem item, Kvs_Type def) {
  switch (settings_get_kvsType(item)) {
  case CBT_u32:
    return static_cast<Kvs_Type>(config_read_item_u32(settings_get_kvsKey(item), (uint32_t) def));
  case CBT_i8:
    return static_cast<Kvs_Type>(config_read_item_i8(settings_get_kvsKey(item), (int8_t) def));
  case CBT_f:
    return static_cast<Kvs_Type>(config_read_item_f(settings_get_kvsKey(item), (float) def));
  default:
    assert(!"unhandled type");
  }
  return def;
}

template<typename Kvs_Type, typename CfgItem>
constexpr const Kvs_Type* config_read_item(CfgItem item, Kvs_Type *d, size_t d_size, const Kvs_Type *def) {
  switch (settings_get_kvsType(item)) {
  case CBT_str:
    return static_cast<const Kvs_Type *>(config_read_item_s(settings_get_kvsKey(item), d, d_size, (const char *)def));
  case CBT_blob:
    return static_cast<const Kvs_Type *>(config_read_item_b(settings_get_kvsKey(item), d, d_size, (const void *)def));
  default:
    assert(!"unhandled type");
  }
  return def;
}

#if 0
template<typename Sett, typename Kvs_Type>
constexpr bool config_save_item(const Sett &settings, Kvs_Type val) {
  switch (settings_get_s(item)) {
  case CBT_u32:
    return static_cast<Kvs_Type>(config_read_item_u32(settings_get_kvsKey(item), (uint32_t) def));
  case CBT_i8:
    return static_cast<Kvs_Type>(config_read_item_i8(settings_get_kvsKey(item), (int8_t) def));
  case CBT_f:
    return static_cast<Kvs_Type>(config_read_item_f(settings_get_kvsKey(item), (float) def));
  default:
    assert(!"unhandled type");
  }
  return def;
}
#endif
