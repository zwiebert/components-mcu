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
class Settings : public SettingsBase<CfgItem> {
public:
  using soCfgFunT = void (*)(const UoutWriter &td);
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


