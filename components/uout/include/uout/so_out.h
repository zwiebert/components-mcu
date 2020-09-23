#pragma once

#include "so_target_bits.h"

#define so_jco ((td.tgt() & (SO_TGT_HTTP|SO_TGT_MQTT)) || ((td.tgt() & SO_TGT_CLI) && (td.tgt() & SO_TGT_FLAG_JSON)))
#define so_cco ((td.tgt() & SO_TGT_CLI) && (td.tgt() & SO_TGT_FLAG_TXT))
#define so_mqt ((td.tgt() & SO_TGT_MQTT))
#define so_ws ((td.tgt() & SO_TGT_WS))

#ifndef USE_JSON
#undef so_jco
#define so_jco false
#endif

class SoOut {
public:
  SoOut(const struct TargetDesc &td) :
      myTd(td) {
  }
public:

  void print(const char *key, long val) const;
  void print(const char *key, int val) const;
  void print(const char *key, long long val) const;
  void print(const char *key, unsigned val, int base = 10) const;
  void print(const char *key, float val, int n) const;
  void print(const char *key, const char *val) const;

  void x_close() const;
  void x_open(const char *name) const;

  void root_close() const;
  bool root_open(const char *name) const;

  bool hasOutput_text() const;
  bool hasOutput_json() const;

private:
  const struct TargetDesc &myTd;
};
