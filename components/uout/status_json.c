/*
 * status_json.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */


#include "uout/status_json.h"

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

static int json_idx;
static char *json_buf;
static u16 json_buf_size;

#define BUF (json_buf+0)
#define BUF_SIZE (json_buf_size)
#define BUF_MAX_SIZE 1024

#ifdef USE_SJ_WRITE
int (*sj_write)(const char *src, unsigned len);

static void sj_write_out_buf() {
  if (!sj_write)
    return;

  int written = sj_write(json_buf, json_idx);
  if (written == json_idx) {
    json_idx = 0;
    return;
  }
}
#else
#define sj_write_out_buf()
#endif

char *sj_get_json() { return BUF; }

bool sj_realloc_buffer(size_t buf_size) {
  precond(buf_size > json_idx);
  if (buf_size > BUF_MAX_SIZE)
    return false;

  void *m = realloc(json_buf, buf_size);

  assert(m); //XXX
  if (!m) {
    return false;
    io_puts("ERROR: sj_realloc_buffer: out of memory\n");
  }

  json_buf = m;
  json_buf_size = buf_size;
  return true;
}

void sj_free_buffer() {
  precond(json_buf);
  free(json_buf);
  json_buf = 0;
  json_buf_size = 0;
}

bool sj_buffer_grow() {
   size_t new_size = 0;

  if (json_buf_size == 0) {
    new_size = 128;
  } else {
    new_size = json_buf_size * 2;
  }
  D(db_printf("sj_buffer_grow to %d\n", (int)new_size));
  return sj_realloc_buffer(new_size);
}



static bool sj_not_enough_buffer(const char *key, const char *val) {
  size_t required_size = strlen(key);
  required_size += val ? strlen(val) : 10;
  required_size += 10;

  if (BUF_SIZE < json_idx + required_size)
    return !sj_buffer_grow();

  postcond(BUF);
  return false;
}

bool sj_copy_to_buf(const char *s) {
  if (sj_not_enough_buffer(s, 0))
    return false;
  strcpy(BUF, s);
  json_idx = strlen(s);
  return true;
}

bool sj_cat_to_buf(const char *s) {
  if (sj_not_enough_buffer(s, 0))
    return false;
  strcat(BUF + json_idx, s);
  json_idx += strlen(s);
  return true;
}

bool sj_open_root_object(const char *id) {
  D(db_printf("%s()\n", __func__));
  precond(json_idx == 0);
  json_idx = 0;
  if (sj_not_enough_buffer(id, 0))
    return false;

  strcat(strcat(strcpy(BUF + json_idx, "{\"from\":\""), id), "\",");
  json_idx += strlen(BUF + json_idx);

  postcond(json_idx > 0);
  return true;
}

bool sj_add_object(const char *key) {
  D(db_printf("%s(%s)\n", __func__, key));
  precond(sj_write || json_idx > 0);
  if (sj_not_enough_buffer(key, 0))
    return false;

  strcat(strcat(strcpy(BUF + json_idx, "\""), key), "\":{");
  json_idx += strlen(BUF + json_idx);
  sj_write_out_buf();
  return true;
}


void sj_close_object() {
  D(db_printf("%s()\n", __func__));
  precond(BUF);
  precond(sj_write || json_idx > 0);
  if (BUF[json_idx - 1] == ',') { // remove trailing comma...
    --json_idx;
  }
  strcpy(BUF + json_idx, "},");
  json_idx += strlen(BUF + json_idx);
}

bool sj_add_array(const char *key) {
  D(db_printf("%s(%s)\n", __func__, key));
  precond(sj_write || json_idx > 0);
  sj_write_out_buf();
  if (sj_not_enough_buffer(key, 0))
    return false;

  strcat(strcat(strcpy(BUF + json_idx, "\""), key), "\":[");
  json_idx += strlen(BUF + json_idx);
  return true;
}

void sj_close_array() {
  D(db_printf("%s()\n", __func__));
  precond(BUF);
  precond(sj_write || json_idx > 0);
  if (BUF[json_idx - 1] == ',') { // remove trailing comma...
    --json_idx;
  }
  strcpy(BUF + json_idx, "],");
  json_idx += strlen(BUF + json_idx);
}

void sj_close_root_object() {
  D(db_printf("%s()\n", __func__));
  precond(BUF);
  precond(sj_write || json_idx > 0);
  if (BUF[json_idx - 1] == ',') { // remove trailing comma...
    --json_idx;
  }
  strcpy(BUF + json_idx, "}");
  ++json_idx;
  sj_write_out_buf();
  json_idx = 0;
}

bool sj_add_value_d(int val) {
  D(db_printf("%s(%d)\n", __func__, val));
  precond(sj_write || json_idx > 0);
  sj_write_out_buf();
  char buf[20];
  ltoa(val, buf, 10);

  if (sj_not_enough_buffer("", buf))
    return false;

  strcat(strcpy(BUF + json_idx, buf), ",");

  json_idx += strlen(BUF + json_idx);
  D(db_printf("json_idx: %u, buf: %s\n", json_idx, BUF));

  return true;
}

bool sj_add_key_value_pair_f(const char *key, float val) {
  D(db_printf("%s(%s, %f)\n", __func__, key, val));
  precond(sj_write || json_idx > 0);;
  precond(key);
  sj_write_out_buf();
  if (sj_not_enough_buffer(key, 0))
    return false;

  char buf[20];
  ftoa(val, buf, 5);
  strcat(strcat(strcat(strcat(strcpy(BUF + json_idx, "\""), key), "\":"), buf), ",");

  json_idx += strlen(BUF + json_idx);
  D(ets_printf("json_idx: %u, buf: %s\n", json_idx, BUF));

  return true;
}

bool sj_add_key_value_pair_d(const char *key, int val) {
  D(db_printf("%s(%s, %d)\n", __func__, key, val));
  precond(sj_write || json_idx > 0);
  precond(key);
  sj_write_out_buf();
  if (sj_not_enough_buffer(key, 0))
    return false;

  char buf[20];
  ltoa(val, buf, 10);
  strcat(strcat(strcat(strcat(strcpy(BUF + json_idx, "\""), key), "\":"), buf), ",");

  json_idx += strlen(BUF + json_idx);
  D(db_printf("json_idx: %u, buf: %s\n", json_idx, BUF));

  return true;
}

bool sj_add_key_value_pair_s(const char *key, const char *val) {
  D(db_printf("%s(%s, %s)\n", __func__, key, val));
  precond(sj_write || json_idx > 0);
  precond(key);
  sj_write_out_buf();
  if (sj_not_enough_buffer(key, val))
    return false;

  strcat(strcat(strcat(strcat(strcpy(BUF + json_idx, "\""), key), "\":\""), val), "\",");

  json_idx += strlen(BUF + json_idx);
  D(ets_printf("json_idx: %u, buf: %s\n", json_idx, BUF));

  return true;
}

