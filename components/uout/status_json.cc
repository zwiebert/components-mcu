/*
 * status_json.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "uout/status_json.hh"

#include "uout/status_output.h"
#include "misc/int_macros.h"
#include "txtio/inout.h"
#include "debug/dbg.h"

#include <string.h>
#include <stdlib.h>
#include "userio_app_cfg.h"
#include "misc/int_types.h"
#include "misc/ftoa.h"
#include "misc/itoa.h"

#define D(x)

#define BUF_MAX_SIZE 1024
#define unused_write_out_buf()

#define myBuf_freeSize (myBuf_size - myBuf_idx)
#define myBuf_cursor (myBuf + myBuf_idx)

///////////////////////////////////////Private/////////////////////////////
bool StatusJsonT::realloc_buffer(size_t buf_size) {
  precond(buf_size > myBuf_idx);
  if (buf_size > BUF_MAX_SIZE)
    return false;

  void *m = realloc(myBuf, buf_size);

  assert(m); //XXX
  if (!m) {
    return false;
    io_puts("ERROR: StatusJsonT::realloc_buffer: out of memory\n");
  }

  myBuf = static_cast<char*>(m);
  myBuf_size = buf_size;
  return true;
}

void StatusJsonT::free_buffer() {
  precond(myBuf);
  if (!myBuf_isMine)
    return;

  free(myBuf);
  myBuf = 0;
  myBuf_size = 0;
}

bool StatusJsonT::buffer_grow() {
  if (!myBuf_isMine)
    return false;

  size_t new_size = 0;

  if (myBuf_size == 0) {
    new_size = 128;
  } else {
    new_size = myBuf_size * 2;
  } D(db_printf("sj_buffer_grow to %d\n", (int)new_size));
  return realloc_buffer(new_size);
}

//////////////////////////////////////Public/////////////////////////////////////////////
StatusJsonT::~StatusJsonT() {
  if (myBuf_isMine)
    free(myBuf);
}

char* StatusJsonT::get_json() const {
  return myBuf;
}


bool StatusJsonT::not_enough_buffer(const char *key, const char *val) {
  size_t required_size = strlen(key);
  required_size += val ? strlen(val) : 10;
  required_size += 10;

  if (myBuf_size < myBuf_idx + required_size)
    return !buffer_grow();

  postcond(myBuf);
  return false;
}

bool StatusJsonT::copy_to_buf(const char *s) {
  if (not_enough_buffer(s, 0))
    return false;
  STRCPY(myBuf, s);
  myBuf_idx = strlen(s);
  return true;
}

bool StatusJsonT::cat_to_buf(const char *s) {
  if (not_enough_buffer(s, 0))
    return false;
  strcat(myBuf + myBuf_idx, s);
  myBuf_idx += strlen(s);
  return true;
}

bool StatusJsonT::open_root_object(const char *id) {
  D(db_printf("%s()\n", __func__));
  myBuf_idx = 0;
  if (not_enough_buffer(id, 0))
    return false;

  myBuf_idx +=  csu_copy_cat(myBuf_cursor, myBuf_freeSize, "{\"from\":\"", id, "\",");

  postcond(myBuf_idx > 0);
  return true;
}

int StatusJsonT::add_object(const char *key) {
  int result = myBuf_idx;
  D(db_printf("%s(%s)\n", __func__, key));
  precond(myBuf_idx > 0);
  if (not_enough_buffer(key, 0))
    return -1;

  myBuf_idx +=  csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":{");
  unused_write_out_buf();

  postcond(myBuf_size > myBuf_idx);
  return result;
}

void StatusJsonT::close_object() {
  D(db_printf("%s()\n", __func__));
  precond(myBuf);
  precond(myBuf_idx > 0);
  if (myBuf[myBuf_idx - 1] == ',') { // remove trailing comma...
    --myBuf_idx;
  }
  STRCPY(myBuf + myBuf_idx, "},");
  myBuf_idx += strlen(myBuf + myBuf_idx);
  postcond(myBuf_size > myBuf_idx);
}

bool StatusJsonT::add_array(const char *key) {
  D(db_printf("%s(%s)\n", __func__, key));
  precond(myBuf_idx > 0); unused_write_out_buf();
  if (not_enough_buffer(key, 0))
    return false;

  myBuf_idx +=  csu_copy_cat(myBuf_cursor, myBuf_freeSize,  "\"", key, "\":[");

  postcond(myBuf_size > myBuf_idx);
  return true;
}

void StatusJsonT::close_array() {
  D(db_printf("%s()\n", __func__));
  precond(myBuf);
  precond(myBuf_idx > 0);
  if (myBuf[myBuf_idx - 1] == ',') { // remove trailing comma...
    --myBuf_idx;
  }
  myBuf_idx +=  csu_copy_cat(myBuf_cursor, myBuf_freeSize, "],");
  postcond(myBuf_size > myBuf_idx);
}

void StatusJsonT::close_root_object() {
  D(db_printf("%s()\n", __func__));
  precond(myBuf);
  precond(myBuf_idx > 0);
  if (myBuf[myBuf_idx - 1] == ',') { // remove trailing comma...
    --myBuf_idx;
  }

  myBuf_idx +=  csu_copy_cat(myBuf_cursor, myBuf_freeSize, "}");
  unused_write_out_buf();
}

bool StatusJsonT::add_value_d(int val) {
  D(db_printf("%s(%d)\n", __func__, val));
  precond(myBuf_idx > 0); unused_write_out_buf();
  char buf[20];
  ltoa(val, buf, 10);

  if (not_enough_buffer("", buf))
    return false;

  myBuf_idx +=  csu_copy_cat(myBuf_cursor, myBuf_freeSize,  ",");

  D(db_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool StatusJsonT::add_key_value_pair_f(const char *key, float val) {
  D(db_printf("%s(%s, %f)\n", __func__, key, val));
  precond(myBuf_idx > 0);;
  precond(key); unused_write_out_buf();
  if (not_enough_buffer(key, 0))
    return false;

  char buf[20];
  ftoa(val, buf, 5);
  myBuf_idx +=  csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":", buf, ",");
  D(ets_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool StatusJsonT::add_key_value_pair_d(const char *key, int val) {
  D(db_printf("%s(%s, %d)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key); unused_write_out_buf();
  if (not_enough_buffer(key, 0))
    return false;

  char buf[20];
  ltoa(val, buf, 10);
  myBuf_idx +=  csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":", buf, ",");
  D(db_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool StatusJsonT::add_key_value_pair_s(const char *key, const char *val) {
  D(db_printf("%s(%s, %s)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key); unused_write_out_buf();
  if (not_enough_buffer(key, val))
    return false;

  myBuf_idx +=  csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":\"", val, "\",");
  D(ets_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

int StatusJsonT::writeln_json(bool final) const {
  if (!(myTd && (myTd->tgt() & SO_TGT_FLAG_JSON)))
    return -1;
  if (!myBuf_idx) {
    const char json[] = "{}\n";
    return myTd->write(json, sizeof json, final);
  }

  if (char *json = myBuf) {
    if (myBuf_idx < myBuf_size) {
      myBuf[myBuf_idx] = '\n';
      return myTd->write(json, myBuf_idx + 1, final);
    } else {
      int n = myTd->write(json, myBuf_idx, false);
      n += myTd->write("\n", 1, final);
      return n;
    }
  }
  return -1;
}

int StatusJsonT::write_json(bool final) const {
  if (!(myTd && (myTd->tgt() & SO_TGT_FLAG_JSON)))
    return -1;
  if (!myBuf_idx) {
    const char json[] = "{}";
    return myTd->write(json, sizeof json, final);
  }

  if (const char *json = myBuf) {
    int n = myTd->write(json, myBuf_idx, final);
    return n;
  }
  return -1;
}
