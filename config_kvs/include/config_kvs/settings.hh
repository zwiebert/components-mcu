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
  virtual const SettingsData* get_SettingsData(const otok optKey) const = 0;

  virtual soCfgFunT get_soCfgFun(const otok optKey) const = 0;
  virtual StoreFun get_storeFun(const otok optKey) const = 0;
  virtual const char* get_kvsKey(const otok optKey) const = 0;
  virtual KvsType get_kvsType(const otok optKey) const = 0;
  virtual uint64_t get_bitMask(const otok optKey) const {
    return 0;
  }
};

template<typename CfgItem>
class SettingsBase: public Settings1stBase {

public:
  using soCfgFunT = void (*)(UoutWriter &td);
  using storeFunOkT = void (*)(otok key, const char *val);

public:

  template<typename config_item_type>
  const SettingsData* get_SettingsData(const config_item_type item) const {
    return get_SettingsData(item);
  }
  virtual const SettingsData* get_SettingsData(const CfgItem item) const = 0;
  virtual const SettingsData* get_SettingsData(const otok optKey) const {
    return get_SettingsData(get_item(optKey));
  }

  virtual CfgItem get_item(const otok optKey) const = 0;
  virtual uint64_t get_bitMask(const otok optKey) const {
    int n = (int) get_item(optKey);
    return n >= 0 ? BIT64(n) : 0;
  }

  template<typename config_item_type>
  const char* get_kvsKey(const config_item_type item) const {
    return get_kvsKey(static_cast<const CfgItem>(item));
  }
  virtual const char* get_kvsKey(const CfgItem item) const = 0;
  virtual const char* get_kvsKey(const otok optKey) const {
    return get_kvsKey(get_item(optKey));
  }

  template<typename config_item_type>
  KvsType get_kvsType(const config_item_type item) const {
    return get_kvsType(static_cast<const CfgItem>(item));
  }
  virtual KvsType get_kvsType(const CfgItem item) const = 0;
  virtual KvsType get_kvsType(const otok optKey) const {
    return get_kvsType(get_item(optKey));
  }

  template<typename config_item_type>
  otok get_optKey(const config_item_type item) const {
    return get_optKey(static_cast<const CfgItem>(item));
  }
  virtual otok get_optKey(const CfgItem item) const = 0;

  template<typename config_item_type>
  const char* get_optKeyStr(const config_item_type item) const {
    return get_optKeyStr(static_cast<const CfgItem>(item));
  }
  virtual const char* get_optKeyStr(const CfgItem item) const = 0;
  virtual const char* get_optKeyStr(const otok optKey) const {
    return get_optKeyStr(get_item(optKey));
  }

  template<typename config_item_type>
  soCfgFunT get_soCfgFun(const config_item_type item) const {
    return get_soCfgFun(static_cast<const CfgItem>(item));
  }
  virtual soCfgFunT get_soCfgFun(const CfgItem item) const = 0;
  virtual soCfgFunT get_soCfgFun(const otok optKey) const = 0;

  template<typename config_item_type>
  StoreFun get_storeFun(const config_item_type item) const {
    return get_storeFun(static_cast<const CfgItem>(item));
  }
  virtual StoreFun get_storeFun(const CfgItem item) const = 0;
  virtual StoreFun get_storeFun(const otok optKey) const = 0;

};
