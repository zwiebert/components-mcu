/**
 * \file   uout/so_out.h
 * \brief  forwards output according to the flags in target descriptor
 */
#pragma once

#include "so_target_bits.h"

#define so_jco ((td.tgt() & (SO_TGT_HTTP|SO_TGT_MQTT|SO_TGT_WS)) || ((td.tgt() & SO_TGT_CLI) && (td.tgt() & SO_TGT_FLAG_JSON)))
#define so_cco ((td.tgt() & SO_TGT_CLI) && (td.tgt() & SO_TGT_FLAG_TXT))
#define so_mqt ((td.tgt() & SO_TGT_MQTT))
#define so_ws ((td.tgt() & SO_TGT_WS))

#ifndef USE_JSON
#undef so_jco
#define so_jco false
#endif

/**
 * \brief Format and print output according to the flags in the target descriptor.
 */
class SoOut {
public:
  /**
   * \param td       Target descriptor which describes the wanted output formats and may hold a write function to print the output immediately
   */
  SoOut(const struct TargetDesc &td) :
      myTd(td) {
  }
public:

  void print(const char *key, long val) const;  ///< forward print of key/val pair
  void print(const char *key, int val) const; ///< forward print of key/val pair
  void print(const char *key, long long val) const; ///< forward print of key/val pair
  void print(const char *key, unsigned val, int base = 10) const; ///< forward print of key/val pair
  void print(const char *key, unsigned long val, int base = 10) const; ///< forward print of key/val pair
  void print(const char *key, float val, int n) const; ///< forward print of key/val pair
  void print(const char *key, const char *val) const; ///< forward print of key/val pair

  void x_open(const char *name) const; ///< start an object like "name":{ in JSON
  void x_close() const;  ///< close an object like } in JSON

  bool root_open(const char *name) const; ///< start the root object like {"from":"tfmcu",  in JSON
  void root_close() const;  ///< close the root object like } in JSON


  bool hasOutput_text() const;  ///< test if target descriptor wants output in text format
  bool hasOutput_json() const; ///< test if target descriptor wants output in JSON format

private:
  const struct TargetDesc &myTd;
};
