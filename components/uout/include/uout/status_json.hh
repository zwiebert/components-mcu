#pragma once

#include <stdbool.h>
#include <stdlib.h>


class StatusJsonT {
public:
  StatusJsonT(char *buf, size_t buf_size, const struct TargetDesc *td = 0) :
      myBuf(buf), myBuf_size(buf_size), myBuf_isMine(false), myTd(td) {
  }
  StatusJsonT(TargetDesc *td = 0) :
      myTd(td) {
  }
  ~StatusJsonT();

public:
  bool open_root_object(const char *id);
  int add_object(const char *key);
  bool add_key_value_pair_s(const char *key, const char *val);
  bool add_key_value_pair_d(const char *key, int val);
  bool add_key_value_pair_f(const char *key, float val);

  bool add_array(const char *key);
  void close_array();
  bool add_value_d(int val);

  void close_object();
  void close_root_object();

  // copy/cat some json int buf
  bool copy_to_buf(const char *s);
  bool cat_to_buf(const char *s);

  char* get_json() const;
  int writeln_json(bool final = true) const;
  int write_json(bool final = true) const;
  void free_buffer();

private:
  bool realloc_buffer(size_t buf_size);
  bool not_enough_buffer(const char *key, const char *val);
  bool buffer_grow();

  char *myBuf = 0;
  size_t myBuf_size = 0;
  int myBuf_idx = 0;
  bool myBuf_isMine = true;
  const struct TargetDesc *myTd = 0;
};


