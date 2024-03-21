/**
 * \file uout/status_json.hh
 * \brief  Create JSON output from key/val pairs
 */
#pragma once

#include <stdlib.h>

/**
 * \brief Build JSON output gradually by calling member functions (with key/value parameters).
 */
class UoutBuilderJson {
public:
  UoutBuilderJson(char *buf, size_t buf_size, const class UoutWriter *td = 0) :
      myBuf(buf), myBuf_size(buf_size), myBuf_isMine(false), myTd(td) {
  }
  UoutBuilderJson(UoutWriter *td = 0) :
      myTd(td) {
  }
  ~UoutBuilderJson();

public:
  bool open_root_object(const char *id); ///< \brief add root object  {"from":"ID",
  void close_root_object(); ///< \brief close the root object with },

  int add_object(const char *key); ///< \brief add object "KEY":{
  int add_object(); ///< \brief add object w/o a key (e.g. in arrays):{
  void close_object();  ///< \brief close object with },

  bool add_key_value_pair_s(const char *key, const char *val); ///< \brief  Add key value pair "KEY":"VAL"
  bool add_key_value_pair_d(const char *key, int val); ///< \brief  Add key value pair "KEY":VAL
  bool add_key_value_pair_f(const char *key, float val, int prec = 5); ///< \brief  Add key value pair "KEY":VAL

  bool add_array(const char *key);  ///< \brief  add array  "KEY":[
  void close_array(); ///< \brief close array with ],
  bool add_value_d(int n);  ///< \brief Add number to array: N,
  bool add_value_s(const char *val); //< \brief Add string to array,




  // copy/cat some json int buf
  bool copy_to_buf(const char *s); ///< \brief Copy some external JSON into this object's buffer (buffer will be overwritten)
  bool cat_to_buf(const char *s); ///< \brief Append some external JSON to this object's buffer

  char* get_json() const; ///< \brief  Get this objects JSON as null terminated string (ownership remains to this object)
  int writeln_json(bool final = true); ///< \brief write this objects JSON as line to its Target descriptor
  int write_json(bool final = true); ///< \brief write this objects JSON to its Target descriptor w/o newline
  int write_some_json(); ///< \brief write buffer to its Target (final=false), but leave the last character in the buffer at index 0.
  void free_buffer(); ///<  Destroy this objects JSON buffer (Optional. Destructor will take care of it)

private:
  bool realloc_buffer(size_t buf_size);
  bool not_enough_buffer(const char *key, const char *val);
  bool buffer_grow();

  char *myBuf = 0;
  size_t myBuf_size = 0;
  int myBuf_idx = 0;
  bool myBuf_isMine = true;
  int m_obj_ct = 0;
  const class UoutWriter *myTd = 0;
};


