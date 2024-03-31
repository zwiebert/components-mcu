/**
 * \file config_kvs/settings.hh
 */

#pragma once

#include "settings.hh"

/**
 * \brief            Base class template for settings
 * \param CfgItem    Enumeration of configurations
 * \param Size       number of config items
 * \param Offset     If enumeration does not start at zero, provide an offset here
 */
template<typename CfgItem, size_t Size, size_t Offset = 0>
class Settings: public SettingsBase<CfgItem> {
public:
  using Item = CfgItem;
  using Base = SettingsBase<CfgItem>;
  using soCfgFunT = Base::soCfgFunT;
  using storeFunT = void (*)(CfgItem item, const char *val);
  using ourT = Settings<CfgItem, Size, Offset>;
  using initFunT = void (*)(ourT&);


public:
  virtual const SettingsData* get_SettingsData(const CfgItem item) const {
    if ((int) item < 0)
      return nullptr;

    return &m_data[item - Offset];
  }

  constexpr const char* get_kvsKey(const CfgItem item) const {
    if ((int) item < 0)
      return nullptr;

    return m_data[item - Offset].kvs_key;
  }
  constexpr KvsType get_kvsType(const CfgItem item) const {
    if ((int) item < 0)
      return CBT_none;

    return m_data[item - Offset].kvs_type;
  }

  constexpr otok get_optKey(const CfgItem item) const {
    if ((int) item < 0)
      return otok::NONE;

    return m_data[item - Offset].opt_key;
  }
  constexpr const char* get_optKeyStr(const CfgItem item) const {
    if ((int) item < 0)
      return nullptr;

    return otok_strings[static_cast<otokBaseT>(m_data[item - Offset].opt_key)];
  }

  constexpr soCfgFunT get_soCfgFun(const CfgItem item) const {
    if ((int) item < 0)
      return nullptr;

    return m_data[item - Offset].so_cfg_fun;
  }

  constexpr soCfgFunT get_soCfgFun(const otok optKey) const {
    if (auto idx = otok2idx(optKey); idx >= 0)
      return m_data[idx].so_cfg_fun;
    return nullptr;
  }

  constexpr StoreFun get_storeFun(const CfgItem item) const {
    if ((int) item < 0)
      return STF_none;

    return m_data[item - Offset].store_fun;
  }

  constexpr StoreFun get_storeFun(const otok optKey) const {
    if (auto idx = otok2idx(optKey); idx >= 0)
      return m_data[idx].store_fun;
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
        if (optKey == m_data[i].opt_key)
          return i;
      }
    return -1;
  }

public:
  constexpr void initField(const CfgItem ai, const char *const kvsKey, const otok optKey, const KvsType kvsType, soCfgFunT soCfgFun = 0, StoreFun storeFun =
      STF_none) {
    uint8_t idx = ai - Offset;
    m_data[idx] = SettingsData { .kvs_key = kvsKey, .so_cfg_fun = soCfgFun, .opt_key = optKey, .kvs_type = kvsType, .store_fun = storeFun, .id_bit = idx };
  }
protected:
  SettingsData m_data[Size] = {};
};


