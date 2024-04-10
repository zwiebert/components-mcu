/**
 * \file   uout/so_out.h
 * \brief  write formatted output to UoutWriter
 *
 */

#pragma once

#include "so_target_bits.h"

//#define so_jco ((td.tgt() & (SO_TGT_HTTP|SO_TGT_MQTT|SO_TGT_WS)) || ((td.tgt() & SO_TGT_CLI) && (td.tgt() & SO_TGT_FLAG_JSON)))
#define so_jco (td.tgt() & SO_TGT_FLAG_JSON)
#define so_cco ((td.tgt() & SO_TGT_CLI) && (td.tgt() & SO_TGT_FLAG_TXT))
#define so_mqt ((td.tgt() & SO_TGT_MQTT))
#define so_ws ((td.tgt() & SO_TGT_WS))

/**
 * \brief Format and print output using the help of \ref  UoutWriter
 */
class UoutPrinter {
public:
  /**
   * \param td       writer which holds the actual builders \ref UoutWriter::mySj, \ref UoutWriter::myStxt and the output format in \ref UoutWriter::myTgt
   *                 Supported output formats are: SO_TGT_FLAG_TXT, SO_TGT_FLAG_JSON.
   */
  UoutPrinter(class UoutWriter &td) :
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
  const class UoutWriter &myTd;
};
