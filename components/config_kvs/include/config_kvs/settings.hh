/**
 * \file config_kvs/settings.hh
 */

#pragma once
#include "app_config/proj_app_cfg.h"
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

/**
 * \brief            Base class template for settings
 * \param CfgItem    Enumeration of configurations
 * \param Size       number of config items
 * \param Offset     If enumeration does not start at zero, provide an offset here
 */
template<typename CfgItem, size_t Size, size_t Offset = 0>
class Settings {
public:
  using soCfgFunT = void (*)(const TargetDesc &td);
  using storeFunOkT = void (*)(otok key, const char *val);
public:
  constexpr const char* get_kvsKey(const CfgItem item) const {
    return m_kvsKeys[item - Offset];
  }
  constexpr KvsType get_kvsType(const CfgItem item) const {
    return m_kvsTypes[item - Offset];
  }

  constexpr otok get_optKey(const CfgItem item) const {
    return m_optKeys[item - Offset];
  }
  constexpr const char* get_optKeyStr(const CfgItem item) const {
    return otok_strings[static_cast<otokBaseT>(m_optKeys[item - Offset])];
  }

  constexpr soCfgFunT get_soCfgFun(const CfgItem item) const {
    return m_soCfgFuns[item - Offset];
  }

  constexpr soCfgFunT get_soCfgFun(const otok optKey) const {
    if (auto idx = otok2idx(optKey); idx >= 0)
      return m_soCfgFuns[idx];
    return nullptr;
  }

  constexpr StoreFun get_storeFun(const CfgItem item) const {
      return m_storeFuns[item - Offset];
  }

  constexpr StoreFun get_storeFun(const otok optKey) const {
    if (auto idx = otok2idx(optKey); idx >= 0)
      return m_storeFuns[idx];
    return STF_none;
  }

  constexpr CfgItem get_item(const otok optKey) const {
      if (auto idx = otok2idx(optKey); idx >= 0)
        return static_cast<CfgItem>(idx + Offset);
    return static_cast<CfgItem>(-1);
  }

protected:
  constexpr int otok2idx(const otok optKey) const {
    if (optKey != otok::NONE)
      for (int i = 0; i < Size; ++i) {
        if (optKey == m_optKeys[i])
          return i;
      }
    return -1;
  }

protected:
  constexpr void initField(const CfgItem ai, const char *const kvsKey, const otok optKey, const KvsType kvsType, soCfgFunT soCfgFun = 0, StoreFun storeFun = STF_none) {
    unsigned idx = ai - Offset;
    m_optKeys[idx] = optKey;
    m_kvsKeys[idx] = kvsKey;
    m_kvsTypes[idx] = kvsType;
    m_soCfgFuns[idx] = soCfgFun;
    m_storeFuns[idx] = storeFun;
  }
protected:
  const char *m_kvsKeys[Size] { };
  soCfgFunT m_soCfgFuns[Size] { };
  otok m_optKeys[Size] { };
  KvsType m_kvsTypes[Size] { };
  StoreFun m_storeFuns[Size] { };
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
