/**
 * \file config_kvs/settings.hh
 */

#pragma once

#include <app_config/options.hh>
#include <uout/so_target_desc.hh>
#include <utils_misc/int_macros.h>
#include "config.h"
#include <assert.h>

enum KvsType : uint8_t {
  CBT_none, CBT_i8, CBT_u8, CBT_i16, CBT_u16, CBT_i32, CBT_u32, CBT_i64, CBT_u64, CBT_str, CBT_f, CBT_blob, CBT_end
};

enum StoreFun : uint8_t {
  STF_none, STF_direct, STF_direct_hex, STF_direct_bool
};

struct SettingsData {
  using soCfgFunT = void (*)(UoutWriter &td);
  using storeFunOkT = void (*)(otok key, const char *val);
  const char *kvs_key;
  soCfgFunT so_cfg_fun;
  otok opt_key;
  KvsType kvs_type;
  StoreFun store_fun;
  uint8_t id_bit;
};


class Settings1stBase {

public:
  using soCfgFunT = SettingsData::soCfgFunT;
  using storeFunOkT = SettingsData::storeFunOkT;

public:
  virtual const SettingsData *get_SettingsData(const otok optKey) const = 0;

  virtual soCfgFunT get_soCfgFun(const otok optKey) const = 0;
  virtual StoreFun get_storeFun(const otok optKey) const = 0;
  virtual const char* get_kvsKey(const otok optKey) const = 0;
  virtual KvsType get_kvsType(const otok optKey) const = 0;
  virtual uint64_t get_bitMask(const otok optKey) const { return 0; }
};


template<typename CfgItem>
class SettingsBase : public Settings1stBase{

public:
  using soCfgFunT = void (*)(UoutWriter &td);
  using storeFunOkT = void (*)(otok key, const char *val);

public:
  virtual const SettingsData *get_SettingsData(const CfgItem item) const = 0;
  virtual const SettingsData *get_SettingsData(const otok optKey) const { return get_SettingsData(get_item(optKey)); }

  virtual CfgItem get_item(const otok optKey) const = 0;
  virtual uint64_t get_bitMask(const otok optKey) const { int n = (int)get_item(optKey);  return n >= 0 ? BIT64(n) : 0; }

  virtual const char* get_kvsKey(const CfgItem item) const = 0;
  virtual const char* get_kvsKey(const otok optKey) const { return get_kvsKey(get_item(optKey)); }

  virtual KvsType get_kvsType(const CfgItem item) const = 0;
  virtual KvsType get_kvsType(const otok optKey) const { return get_kvsType(get_item(optKey)); }

  virtual otok get_optKey(const CfgItem item) const = 0;

  virtual const char* get_optKeyStr(const CfgItem item) const = 0;
  virtual const char* get_optKeyStr(const otok optKey) const  { return get_optKeyStr(get_item(optKey)); }

  virtual soCfgFunT get_soCfgFun(const CfgItem item) const = 0;
  virtual soCfgFunT get_soCfgFun(const otok optKey) const = 0;

  virtual StoreFun get_storeFun(const CfgItem item) const = 0;
  virtual StoreFun get_storeFun(const otok optKey) const = 0;

  virtual void initField(const CfgItem item, const char *const kvsKey, const otok optKey, const KvsType kvsType, soCfgFunT soCfgFun = 0, StoreFun storeFun = STF_none) =0;
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
