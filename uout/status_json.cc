/*
 * status_json.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "uout/uout_builder_json.hh"

#include "uout/uo_types.h"
#include "utils_misc/int_macros.h"
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

#define myBuf_freeSize (myBuf_size - myBuf_idx)
#define myBuf_cursor (myBuf + myBuf_idx)

#ifdef CONFIG_REQUIRE_ROOT_OBJECT
#define ROOT_OBJS 1
#else
#define ROOT_OBJS 0
#endif
///////////////////////////////////////Private/////////////////////////////
bool UoutBuilderJson::realloc_buffer(size_t buf_size) {
  precond(buf_size > myBuf_idx);
  if (buf_size > BUF_MAX_SIZE) {
    db_loge(logtag, "%s: maximal buffer size is %u, but requested: %u", __func__, (unsigned)BUF_MAX_SIZE, (unsigned)buf_size);
    return false;
  }

  void *m = realloc(myBuf, buf_size);

  if (!m) {
    db_loge(logtag, "%s: buffer size=%u too small, but allocation of %u bytes failed", __func__, (unsigned)myBuf_size, (unsigned)buf_size);
    return false;
  }

  myBuf = static_cast<char*>(m);
  myBuf[myBuf_idx] = '\0';
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
  if (!myBuf_isMine) {
    db_loge(logtag, "%s: user provided buffer size=%u too small. %u additional buffer space was requested", __func__, (unsigned)myBuf_size, (unsigned)required_free_space);
    return false;
  }

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
bool UoutBuilderJson::alloc_bigger_buffer(size_t total_size) {
  if (total_size <= myBuf_size)
    return true;
  return realloc_buffer(total_size);
}
char* UoutBuilderJson::get_a_buffer(size_t required_size) {
  if ((myBuf_freeSize < required_size + 1) && !buffer_grow(required_size))
    return nullptr;

  return myBuf + myBuf_idx;
}
bool UoutBuilderJson::advance_position(int n) {
  precond(n <= myBuf_idx);
  unsigned pos = myBuf_idx + n;
  if (!(pos <= myBuf_size)) // allow position to be one behind buffer end
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

  if (myBuf_freeSize < required_size)
    return !buffer_grow(required_size);

  postcond(myBuf);
  return false;
}

bool UoutBuilderJson::read_json_from_function(std::function<int(char *buf, size_t buf_size)> f, size_t required_size) {
  // pass-0: try with as parameter provided buffer size
  // pass-1: try size size returned by failed JSON generator function to_json()
  for (unsigned pass = 0, n; pass < 2; ++pass, required_size = (n + 1) - (myBuf_size - myBuf_idx)) {
    if (myBuf_freeSize < required_size && !buffer_grow(required_size)) {
      return false;
    }

    if (n = f(myBuf + myBuf_idx, (myBuf_size - myBuf_idx)); 0 < n && n < (myBuf_size - myBuf_idx)) {
      myBuf_idx += n;
      myBuf[myBuf_idx++] = ',';
      myBuf[myBuf_idx] = '\0';

      postcond(myBuf_size > myBuf_idx);
      return true;

    }
    if (n <= 0)
      return false;
  }
  return false;
}
bool UoutBuilderJson::read_json_arr_from_function(std::function<int(char *buf, size_t buf_size, unsigned arr_idx)> f, unsigned arr_len, size_t required_size) {
  if (add_array()) {
    for (unsigned i = 0; i < arr_len; ++i) {
      if (!read_json_from_function(std::bind(f, std::placeholders::_1, std::placeholders::_2, i), required_size))
        return false;
    }
    close_array();
    return true;
  }
  return false;
}
bool UoutBuilderJson::read_json_arr2_from_function(std::function<int(char *buf, size_t buf_size, unsigned arr_lidx, unsigned arr_ridx)> f, unsigned arr_llen,
    unsigned arr_rlen, size_t required_size) {
  if (add_array()) {
    for (unsigned li = 0; li < arr_llen; ++li) {
      if (add_array()) {
        for (unsigned ri = 0; ri < arr_rlen; ++ri) {
          if (!read_json_from_function(std::bind(f, std::placeholders::_1, std::placeholders::_2, li, ri), required_size))
            return false;
        }
        close_array();
      }
    }
    close_array();
    return true;
  }
  return false;
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

bool UoutBuilderJson::open_root_object() {
  D(db_printf("%s()\n", __func__));
  precond(m_obj_ct == 0);
  myBuf_idx = m_obj_ct = 0;
  if (not_enough_buffer("", 0))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "{");

  ++m_obj_ct;

  postcond(myBuf_idx > 0 && m_obj_ct == 1);
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
  precond(!m_eof && m_obj_ct >= ROOT_OBJS);
  if (not_enough_buffer("", 0))
    return -1;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "{");

  ++m_obj_ct;

  postcond(myBuf_size > myBuf_idx);
  return result;
}

int UoutBuilderJson::add_object(const char *key) {
  int result = myBuf_idx;
  D(db_printf("%s(%s)\n", __func__, key));
  precond(!m_eof && m_obj_ct >= ROOT_OBJS);
  if (not_enough_buffer(key, 0))
    return -1;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":{");

  ++m_obj_ct;

  postcond(myBuf_size > myBuf_idx);
  return result;
}

void UoutBuilderJson::close_object() {
  D(db_printf("%s()\n", __func__));
  precond(myBuf);
  precond(myBuf_idx > 0 && m_obj_ct > ROOT_OBJS);
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
  precond(!m_eof && m_obj_ct >= ROOT_OBJS);

  if (not_enough_buffer(key, 0))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":[");
  ++m_obj_ct;

  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool UoutBuilderJson::add_array() {
  D(db_printf("%s()\n", __func__));
  precond(!m_eof && m_obj_ct >= ROOT_OBJS);

  if (not_enough_buffer())
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "[");
  ++m_obj_ct;

  postcond(myBuf_size > myBuf_idx);
  return true;
}

void UoutBuilderJson::close_array() {
  D(db_printf("%s()\n", __func__));
  precond(myBuf);
  precond(myBuf_idx > 0 && m_obj_ct > ROOT_OBJS );
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

}

bool UoutBuilderJson::add_value_s(const char *val) {
  D(db_printf("%s(%s)\n", __func__, val));
  precond(myBuf_idx > 0);

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
  precond(myBuf_idx > 0);

  char buf[20];
  ltoa(val, buf, 10);

  if (not_enough_buffer("", buf))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, ",");

  D(db_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool UoutBuilderJson::add_value(long int val) {
  D(db_printf("%s(%d)\n", __func__, val));
  precond(myBuf_idx > 0);

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
  precond(myBuf_idx > 0);
  ;
  precond(key);

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
  precond(key);

  if (not_enough_buffer(key, 0))
    return false;

  char buf[20];
  ltoa(val, buf, 10);
  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":", buf, ",");
  D(db_printf("myBuf_idx: %d, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool UoutBuilderJson::add_key_value_pair_s(const char *key, const char *val) {
  D(db_printf("%s(%s, %s)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key);

  if (not_enough_buffer(key, val))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":\"", val, "\",");
  D(ets_printf("myBuf_idx: %d, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}
bool UoutBuilderJson::put_kv(const char *key, bool val) {
  D(db_printf("%s(%s, %d)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key);

  if (not_enough_buffer(key, 0))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":", val ? "true" : "false", ",");
  D(db_printf("myBuf_idx: %d, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}
bool UoutBuilderJson::put_kv(const char *key, unsigned val) {
  D(db_printf("%s(%s, %u)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key);

  if (not_enough_buffer(key, 0))
    return false;

  myBuf_idx += snprintf(myBuf_cursor, myBuf_freeSize, R"("%s":%u,)", key, val);

  D(db_printf("myBuf_idx: %d, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}
bool UoutBuilderJson::put_kv(const char *key, int val) {
  D(db_printf("%s(%s, %d)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key);

  if (not_enough_buffer(key, 0))
    return false;

  myBuf_idx += snprintf(myBuf_cursor, myBuf_freeSize, R"("%s":%d,)", key, val);

  D(db_printf("myBuf_idx: %d, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}
bool UoutBuilderJson::put_kv(const char *key, long unsigned val) {
  D(db_printf("%s(%s, %lu)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key);

  if (not_enough_buffer(key, 0))
    return false;

  myBuf_idx += snprintf(myBuf_cursor, myBuf_freeSize, R"("%s":%lu,)", key, val);

  D(db_printf("myBuf_idx: %d, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}
bool UoutBuilderJson::put_kv(const char *key, long int val) {
  D(db_printf("%s(%s, %ld)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key);

  if (not_enough_buffer(key, 0))
    return false;

  myBuf_idx += snprintf(myBuf_cursor, myBuf_freeSize, R"("%s":%ld,)", key, val);

  D(db_printf("myBuf_idx: %d, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool UoutBuilderJson::put_kv(const char *key, long long unsigned val) {
  D(db_printf("%s(%s, %llu)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key);

  if (not_enough_buffer(key, 0))
    return false;

  myBuf_idx += snprintf(myBuf_cursor, myBuf_freeSize, R"("%s":%llu,)", key, val);

  D(db_printf("myBuf_idx: %d, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}
bool UoutBuilderJson::put_kv(const char *key, long long int val) {
  D(db_printf("%s(%s, %lld)\n", __func__, key, val));
  precond(myBuf_idx > 0);
  precond(key);

  if (not_enough_buffer(key, 0))
    return false;

  myBuf_idx += snprintf(myBuf_cursor, myBuf_freeSize, R"("%s":%lld,)", key, val);

  D(db_printf("myBuf_idx: %d, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

bool UoutBuilderJson::put_kv(const char *key, const char *val) {
  return add_key_value_pair_s(key, val);
}

bool UoutBuilderJson::add_key(const char *key) {
  D(db_printf("%s(%s)\n", __func__, key));
  precond(myBuf_idx > 0);
  precond(key);

  if (not_enough_buffer(key))
    return false;

  myBuf_idx += csu_copy_cat(myBuf_cursor, myBuf_freeSize, "\"", key, "\":");
  D(ets_printf("myBuf_idx: %u, buf: %s\n", myBuf_idx, myBuf));
  postcond(myBuf_size > myBuf_idx);
  return true;
}

int UoutBuilderJson::writeln_json(bool final) {
  if (m_eof)
    return -1;

  if (!(myTd && (myTd->tgt() & SO_TGT_FLAG_JSON)))
    return -1;

  if (!myBuf_idx) {
    const char json[] = "{}\n";
    auto n = myTd->write(json, sizeof json, final);
    myBuf_idx = 0;
    m_eof = final;
    return n;
  }

  if (char *json = myBuf) {
      bool insert_newline = myBuf_freeSize >= 1;
      if (insert_newline)
        myBuf[myBuf_idx++] = '\n';
      int n = myTd->write(json, myBuf_idx, insert_newline ? final : false);

      if (!insert_newline) {
         n += myTd->write("\n", 1, final);
      }

      myBuf[0] = '\0';
    myBuf_idx = 0;
    m_eof = final;
      return n;
    }
  return -1;
}

int UoutBuilderJson::write_json(bool final) {
  if (m_eof)
    return -1;
  if (!(myTd && (myTd->tgt() & SO_TGT_FLAG_JSON)))
    return -1;
  if (!myBuf_idx) {
    const char json[] = "{}";
    auto n = myTd->write(json, -1, final);
    myBuf_idx = 0;
    m_eof = final;
    return n;
  }

  if (const char *json = myBuf) {
    int n = myTd->write(json, myBuf_idx, final);
    myBuf_idx = 0;
    m_eof = final;
    return n;
  }
  return -1;
}

int UoutBuilderJson::write_some_json() {
  if (m_eof)
    return -1;
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

