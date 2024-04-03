/*
 * status_json.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "uout/uout_builder_json.hh"

#include "uout/uo_types.h"
#include "utils_misc/int_macros.h"
#include "txtio/inout.h"
#include "debug/dbg.h"
#include "debug/log.h"

#include <string.h>
#include <utils_misc/cstring_utils.hh>
#include <stdlib.h>
#include "userio_app_cfg.h"
#include "utils_misc/int_types.h"
#include "utils_misc/ftoa.h"
#include "utils_misc/itoa.h"

#ifdef UOUT_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "uout.json_builder"

#define BUF_MAX_SIZE 2048
#define unused_write_out_buf()

#define myBuf_freeSize (myBuf_size - myBuf_idx)
#define myBuf_cursor (myBuf + myBuf_idx)

///////////////////////////////////////Private/////////////////////////////
bool UoutBuilderJson::realloc_buffer(size_t buf_size) {
  precond(buf_size > myBuf_idx);
  if (buf_size > BUF_MAX_SIZE) {
    db_loge(logtag, "%s: maximal buffer size is %d, but requested: %d", __func__, BUF_MAX_SIZE, buf_size);
    return false;
  }

  void *m = realloc(myBuf, buf_size);

  if (!m) {
    return false;
    io_puts("ERROR: UoutBuilderJson::realloc_buffer: out of memory\n");
  }

  myBuf = static_cast<char*>(m);
  myBuf_size = buf_size;
  return true;
}

void UoutBuilderJson::free_buffer() {
  precond(myBuf);
  if (!myBuf_isMine)
    return;

  free(myBuf);
  myBuf = 0;
  myBuf_size = 0;
}

bool UoutBuilderJson::buffer_grow(size_t required_free_space) {
  // Try to free up enough space
  if (myBuf && myBuf_idx) {
    write_some_json();
    if (required_free_space + 20 < myBuf_size - myBuf_idx) // XXX: improve this
      return true;
  }

  // refuse to realloc user provide buffer
  if (!myBuf_isMine)
    return false;

  // realloc() our buffer
  size_t new_size = 0;
  if (required_free_space) {
    new_size = required_free_space + myBuf_idx;
  } else if (myBuf_size == 0) {
    new_size = 128;
  } else {
    new_size = myBuf_size * 2;
  }D(db_printf("sj_buffer_grow to %d\n", (int)new_size));
  return realloc_buffer(new_size);
}

//////////////////////////////////////Public/////////////////////////////////////////////
UoutBuilderJson::~UoutBuilderJson() {
  if (myBuf_isMine)
    free(myBuf);
}

char* UoutBuilderJson::get_json() const {
  return myBuf;
}

char* UoutBuilderJson::get_a_buffer(size_t required_size) {
  if ((myBuf_size < myBuf_idx + required_size + 1) && !buffer_grow(required_size))
    return nullptr;

  return myBuf + myBuf_idx;
}
bool UoutBuilderJson::advance_position(int n) {
  int pos = myBuf_idx + n;
  if (!(0 <= pos && pos <= myBuf_size)) // allow position to be one behind buffer end
    return false;
  myBuf_idx = pos;

  // we want to keep the buffer always null terminated
  if (myBuf_size == myBuf_idx && !buffer_grow(1)) {
    myBuf[myBuf_idx = 0] = '\0';
    return false;
  }

  myBuf[myBuf_idx] = '\0';

  return true;
}

bool UoutBuilderJson::not_enough_buffer(const char *key, const char *val) {
  size_t required_size = strlen(key);
  required_size += val ? strlen(val) : 10;
  required_size += 10;

  if (myBuf_size < myBuf_idx + required_size)
    return !buffer_grow();

  postcond(myBuf);
  return false;
}

int UoutBuilderJson::read_json_from_function(std::function<int(char *buf, size_t buf_size)> f, size_t required_size) {
  if (myBuf_size < myBuf_idx + required_size) {
    if (!buffer_grow(required_size)) {
      return -1;
    }
  }
  int n = f(myBuf + myBuf_idx, myBuf_size - myBuf_idx);

  if (n > 0)
    myBuf_idx += n;
  return n;
}

bool UoutBuilderJson::copy_to_buf(const char *s) {
  if (not_enough_buffer(s, 0))
    return false;
  STRCPY(myBuf, s);
  myBuf_idx = strlen(s);
  return true;
}

bool UoutBuilderJson::cat_to_buf(const char *s) {
  if (not_enough_buffer(s, 0))
    return false;
  strcat(myBuf + myBuf_idx, s);
  myBuf_idx += strlen(s);
  return true;
}

bool UoutBuilderJson::open_root_object(const char *id) {
  D(db_printf("%s()\n", __func__));
  precond(m_obj_ct == 0);
  myBuf_idx = m_obj_ct = 0;
  if (not_enough_buffer(id, 0))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "{\"from\":\"", id, "\",");

  ++m_obj_ct;

  postcond(myBuf_idx > 0 && m_obj_ct == 1);
  return true;
}

int UoutBuilderJson::add_object() {
  int result = myBuf_idx;
  D(db_printf("%s()\n", __func__));
  precond(myBuf_idx > 0 && m_obj_ct > 0);
  if (not_enough_buffer("", 0))
    return -1;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "{");
  unused_write_out_buf();

  ++m_obj_ct;

  postcond(myBuf_size > myBuf_idx);
  return result;
}

int UoutBuilderJson::add_object(const char *key) {
  int result = myBuf_idx;
  D(db_printf("%s(%s)\n", __func__, key));
  precond(myBuf_idx > 0 && m_obj_ct > 0);
  if (not_enough_buffer(key, 0))
    return -1;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":{");
  unused_write_out_buf();

  ++m_obj_ct;

  postcond(myBuf_size > myBuf_idx);
  return result;
}

void UoutBuilderJson::close_object() {
  D(db_printf("%s()\n", __func__));
  precond(myBuf);
  precond(myBuf_idx > 0 && m_obj_ct > 1);
  if (not_enough_buffer())
    abort();

  if (myBuf[myBuf_idx - 1] == ',') { // remove trailing comma...
    --myBuf_idx;
  }
  STRCPY(myBuf + myBuf_idx, "},");
  myBuf_idx += strlen(myBuf + myBuf_idx);
  --m_obj_ct;
  postcond(myBuf_size > myBuf_idx);
}

bool UoutBuilderJson::add_array(const char *key) {
  D(db_printf("%s(%s)\n", __func__, key));
  precond(myBuf_idx > 0 && m_obj_ct > 0);unused_write_out_buf();
  if (not_enough_buffer(key, 0))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":[");
  ++m_obj_ct;

  postcond(myBuf_size > myBuf_idx);
  return true;
}

void UoutBuilderJson::close_array() {
  D(db_printf("%s()\n", __func__));
  precond(myBuf);
  precond(myBuf_idx > 0 && m_obj_ct > 1);
  if (not_enough_buffer())
    abort();

  if (myBuf[myBuf_idx - 1] == ',') { // remove trailing comma...
    --myBuf_idx;
  }
  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "],");
  --m_obj_ct;
  postcond(myBuf_size > myBuf_idx);
}

void UoutBuilderJson::close_root_object() {
  D(db_printf("%s()\n", __func__));
  precond(myBuf && m_obj_ct == 1);
  precond(myBuf_idx > 0);
  if (not_enough_buffer())
    abort();

  if (myBuf[myBuf_idx - 1] == ',') { // remove trailing comma...
    --myBuf_idx;
  }

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "}");
  --m_obj_ct;
  unused_write_out_buf();
}

bool UoutBuilderJson::add_value_s(const char *val) {
  D(db_printf("%s(%s)\n", __func__, val));
  precond(myBuf_idx > 0);unused_write_out_buf();

  if (not_enough_buffer("---", val))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"");
  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, val);
  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\",");

  D(db_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool UoutBuilderJson::add_value_d(int val) {
  D(db_printf("%s(%d)\n", __func__, val));
  precond(myBuf_idx > 0);unused_write_out_buf();
  char buf[20];
  ltoa(val, buf, 10);

  if (not_enough_buffer("", buf))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, ",");

  D(db_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool UoutBuilderJson::add_key_value_pair_f(const char *key, float val, int prec) {
  D(db_printf("%s(%s, %f)\n", __func__, key, val));
  precond(myBuf_idx > 0);;
  precond(key);unused_write_out_buf();
  if (not_enough_buffer(key, 0))
    return false;

  char buf[20];
  ftoa(val, buf, prec);
  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":", buf, ",");
  D(ets_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool UoutBuilderJson::add_key_value_pair_d(const char *key, int val) {
  D(db_printf("%s(%s, %d)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key);unused_write_out_buf();
  if (not_enough_buffer(key, 0))
    return false;

  char buf[20];
  ltoa(val, buf, 10);
  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":", buf, ",");
  D(db_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool UoutBuilderJson::add_key_value_pair_s(const char *key, const char *val) {
  D(db_printf("%s(%s, %s)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key);unused_write_out_buf();
  if (not_enough_buffer(key, val))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":\"", val, "\",");
  D(ets_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool UoutBuilderJson::add_key(const char *key) {
  D(db_printf("%s(%s)\n", __func__, key));
  precond(myBuf_idx > 0);
  precond(key);unused_write_out_buf();
  if (not_enough_buffer(key))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":");
  D(ets_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

int UoutBuilderJson::writeln_json(bool final) {
  if (myBuf_idx < 0)
    return -1; //EOF
  if (!(myTd && (myTd->tgt() & SO_TGT_FLAG_JSON)))
    return -1;
  if (!myBuf_idx) {
    const char json[] = "{}\n";
    auto n = myTd->write(json, sizeof json, final);
    myBuf_idx = final ? -1 : 0;
    return n;
  }

  if (char *json = myBuf) {
    if (myBuf_idx < myBuf_size) {
      myBuf[myBuf_idx] = '\n';
      return myTd->write(json, myBuf_idx + 1, final);
    } else {
      int n = myTd->write(json, myBuf_idx, false);
      n += myTd->write("\n", 1, final);
      myBuf_idx = final ? -1 : 0;
      return n;
    }
  }
  return -1;
}

int UoutBuilderJson::write_json(bool final) {
  if (myBuf_idx < 0)
    return -1; //EOF
  if (!(myTd && (myTd->tgt() & SO_TGT_FLAG_JSON)))
    return -1;
  if (!myBuf_idx) {
    const char json[] = "{}";
    auto n = myTd->write(json, -1, final);
    myBuf_idx = final ? -1 : 0;
    return n;
  }

  if (const char *json = myBuf) {
    int n = myTd->write(json, myBuf_idx, final);
    myBuf_idx = final ? -1 : 0;
    return n;
  }
  return -1;
}

int UoutBuilderJson::write_some_json() {
  if (myBuf_idx < 0)
    return -1; //EOF
  if (!(myTd && (myTd->tgt() & SO_TGT_FLAG_JSON)))
    return -1;
  if (myBuf_idx <= 1) {  // keep one character for un-reading commas
    return 0;
  }

  if (const char *json = myBuf) {
    int n = myTd->write(json, myBuf_idx - 1, false);
    if (n <= 0)
      return n;

    if (n < myBuf_idx)
      memcpy(myBuf, myBuf + n, myBuf_idx - n);

    myBuf_idx -= n;
    return n;
  }
  return -1;
}

