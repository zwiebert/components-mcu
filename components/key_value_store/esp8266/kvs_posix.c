/*
 * kvs_posix.c
 *
 *  Created on: 30.03.2020
 *      Author: bertw
 */
#include "kvs_wrapper.h"

#define TEST_MODULE

#ifndef MCU_ESP8266
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

#define GET_BIT(var,pos) ((var) & (1<<(pos)))
#define SET_BIT(var,pos) ((var) |= (1 << (pos)))
#define CLR_BIT(var,pos) ((var) &= ~((1) << (pos)))
#define PUT_BIT(var,pos, val) ((val) ? SET_BIT(var,pos) : CLR_BIT(var,pos))


#else


#include "app_config/proj_app_cfg.h"
#include "misc/int_types.h"
#include "misc/int_macros.h"
#include "storage/storage.h"
#include "storage/spiffs_fs.h"


#define lseek(fd, pos, whence) SPIFFS_lseek(&fs, fd, pos, whence)
#define read(fd, buf, size) SPIFFS_read(&fs, fd, buf, size)
#define write(fd, buf, size) SPIFFS_write(&fs, fd, buf, size)
#define close(fd) SPIFFS_close(&fs, fd);
#define open(name, flags, mode) SPIFFS_open(&fs, name, flags, mode)

#define SEEK_SET SPIFFS_SEEK_SET
#define SEEK_CUR SPIFFS_SEEK_CUR
#define SEEK_END SPIFFS_SEEK_END
#define O_RDONLY SPIFFS_RDONLY
#define O_RDWR SPIFFS_RDWR
#define O_CREAT SPIFFS_CREAT
#endif

#define D(x) x
#define DT(x) x

struct kvs_handle {
  char *buf;
  int fd;
  unsigned file_size :14;
  unsigned in_use :1;
};

#define MAX_KEY_LEN 14
#define COOKIE 0xbeef
#define ERR_CORRUPT_FILE -42

struct line_info {
  u16 magic;
  u8 kvs_type;
  char key[MAX_KEY_LEN + 1];
  union {
    u8 val_u8;
    i8 val_i8;
    u16 val_u16;
    i16 val_i16;
    u32 val_u32;
    i32 val_i32;
    u16 blob_len, blob_size;
  } nval;
#define li_size(li) (sizeof(*li) + ((li->kvs_type & 0x60) ? li->nval.blob_size : 0))
};

#define HANDLE_COUNT 4
static struct kvs_handle handles[HANDLE_COUNT];

bool kvs_checkMagicCookie(struct line_info *li) {
  if (li->magic == COOKIE)
    return true;

  D(printf("error: corrupt file (bad magic cookie)\n")); // TODO: delete corrupt file
  return false;

}

kvshT kvs_callocHandle() {
  kvshT h = 0;
  int i;

  for (i = 0; i < HANDLE_COUNT; ++i) {
    if (handles[i].in_use)
      continue;
    h = &handles[i];
    h->in_use = true;
    h->file_size = 0;
    h->buf = 0;
  }

  return h;
}

void kvs_freeHandle(kvshT h) {
  h->in_use = false;
}

void kvs_destroyHandle(kvshT h) {
  if (!h)
    return;
  if (h->fd)
    close(h->fd);
  free(h->buf);
  kvs_freeHandle(h);
}

#define VP2H(vp)((vp)->handle)

kvshT kvs_open(const char *name, unsigned flags) {
  i32 res;
  kvshT h;
  int sf = flags == kvs_READ ? O_RDONLY : (O_CREAT | O_RDWR);

  if ((h = kvs_callocHandle())) {
    if ((h->fd = open(name, sf, 0666)) >= 0) {
      res = lseek(h->fd, 0, SEEK_END);
      if (res > 0)
        h->file_size = res;

      return h;
    }
    kvs_destroyHandle(h);
  }
  return 0;
}

int kvs_find_next(kvshT h, struct line_info *li, int pos, const char *key, kvs_type_t kvs_type) {
  DT(printf("%s:(%p, %p, %d, %s, %d)\n", __func__, h, li, pos, key, kvs_type));
  DT(printf("line_info: kvs_type=0x%02x, key=%s 0x%x\n", li->kvs_type, li->key, li->nval.val_u32));
  int len;
  int i;
  if (li->kvs_type == 0) {
    if (lseek(h->fd, pos, SEEK_SET) != pos)
      goto err;
  } else {
    pos += li_size(li);
    ;
    if (lseek(h->fd, pos, SEEK_SET) != pos)
      goto err;
  }

  for (i = 0; read(h->fd, li, sizeof *li) == sizeof *li; ++i) {
    bool found_key = (key && strncmp(li->key, key, MAX_KEY_LEN) == 0);
    bool found_kvs_type = (li->kvs_type == kvs_type);

    if (!kvs_checkMagicCookie(li)) {
      return ERR_CORRUPT_FILE;
    }

    if ((key && !found_key) || (kvs_type != KVS_TYPE_ANY && !found_kvs_type)) {
      if (li->kvs_type & 0x60) {
        pos = lseek(h->fd, li->nval.blob_size, SEEK_CUR);
        continue;
      }
      pos += sizeof *li;
      continue;
    }
    DT(printf("return: %d\n", pos));
    return pos;
  }

  err: return -1;
}

int kvs_write(kvshT h, struct line_info *li, int pos) {
  DT(printf("%s:(%p, %p, %d)\n", __func__, h, li, pos));
  DT(printf("line_info: kvs_type=0x%02x, key=%s 0x%x\n", li->kvs_type, li->key, li->nval.val_u32));

  if (pos >= 0) {
    if (lseek(h->fd, pos, SEEK_SET) != pos)
      goto err;
  } else {
    pos = lseek(h->fd, 0, SEEK_END);
  }

  unsigned size = sizeof *li;

  if (write(h->fd, li, size) != size)
    return -1;

  pos += size;
  return pos;

  err: return -1;
}

bool kvs_commit(kvshT handle) {
  return true;
}

void kvs_close(kvshT handle) {
  kvs_destroyHandle(handle);
}

bool kvs_erase_key(kvshT h, const char *key) {
  struct line_info li = { COOKIE };

  int pos = kvs_find_next(h, &li, 0, key, KVS_TYPE_ANY);

  if (pos >= 0) {
    SET_BIT(li.kvs_type, 7);
    li.key[0] = '\0';
    int res = kvs_write(h, &li, pos);
    pos = -1;
    return true;
  }
  return false;
}

int kvs_foreach(const char *name_space, kvs_type_t type, const char *key_match, kvs_foreach_cbT cb) {
  int pos = 0;
  kvshT h = 0;
  int count = 0;

  if ((h = kvs_open(name_space, kvs_READ))) {
    struct line_info li = { COOKIE };
    while ((pos = kvs_find_next(h, &li, pos, 0, type)) >= 0) {
      kvs_type_t type_found = li.kvs_type;

      if (!kvs_checkMagicCookie(&li)) {
        return ERR_CORRUPT_FILE;
      }

      if (key_match) {
        unsigned len = strlen(key_match);
        if (strlen(li.key) < len)
          continue;
        if (0 != strncmp(li.key, key_match, len))
          continue;
      }

      if (cb) {
        switch (cb(li.key, li.kvs_type)) {
        case kvsCb_match:
          ++count;
          break;
        case kvsCb_done:
          ++count;
          goto succ;
        case kvsCb_noMatch:
          break;
        }
      } else {
        ++count;
      }
    }
    goto succ;
  }

  succ: if (h)
    kvs_close(h);
  return count;
}

//////////////////////// set/get functions //////////////////////////
static int find_key_int(kvshT h, struct line_info *lip, const char *key) {
#define li (*lip)
  int pos = kvs_find_next(h, &li, 0, key, KVS_TYPE_ANY);

  if (pos >= 0) {
    if (li.kvs_type & 0x60) {
      SET_BIT(li.kvs_type, 7);
      li.key[0] = '\0';
      int res = kvs_write(h, &li, pos);
      pos = -1;
    }
  }
#undef li
  return pos;
}



#define SET_DT_FUN(VAL_T) bool kvs_set_##VAL_T(kvshT h, const char *key, VAL_T val) { \
  struct line_info li = { COOKIE }; \
  int pos = find_key_int(h, &li, key); \
  if (pos < 0) { \
    li.kvs_type = KVS_TYPE_##VAL_T; \
    strncpy(li.key, key, MAX_KEY_LEN); \
  } \
  li.nval.val_##VAL_T = val; \
  int res = kvs_write(h, &li, pos); \
  return res > 0; \
}

#define GET_DT_FUN(VAL_T) VAL_T kvs_get_##VAL_T(kvshT h, const char *key, VAL_T default_val, bool *res_success){ \
  struct line_info li = { COOKIE }; \
  int pos = kvs_find_next(h, &li, 0, key, KVS_TYPE_##VAL_T); \
  if (res_success) \
    *res_success = pos >= 0; \
  if (pos < 0) \
    return default_val; \
  return li.nval.val_##VAL_T; \
}

#define SET_GET_DT_FUN(dt) SET_DT_FUN(dt) GET_DT_FUN(dt);

SET_GET_DT_FUN(i8);
SET_GET_DT_FUN(u8);
SET_GET_DT_FUN(i16);
SET_GET_DT_FUN(u16);
SET_GET_DT_FUN(i32);
SET_GET_DT_FUN(u32);

static unsigned kvs_rw_str_or_blob(kvshT h, const char *key, void *src_or_dst, unsigned length, bool do_write, kvs_type_t kvs_type) {
  struct line_info li = { COOKIE };

  if (!do_write) {
    int pos = kvs_find_next(h, &li, 0, key, kvs_type);

    if (pos < 0)
      return 0;

    unsigned len = li.nval.blob_len;
    if (len > length)
      len = length;

    if (read(h->fd, src_or_dst, len) != len) {
      return 0;
    }
    return len;
  }

  if (do_write) {
    int pos = kvs_find_next(h, &li, 0, key, KVS_TYPE_ANY);
    char *src = src_or_dst;
    unsigned src_len = length;

    if (pos >= 0) {
      if (!(li.kvs_type == KVS_TYPE_STR || li.kvs_type == KVS_TYPE_BLOB) || li.nval.blob_size < src_len) {
        SET_BIT(li.kvs_type, 7);
        li.key[0] = '\0';
        int res = kvs_write(h, &li, pos);
        pos = -1;
      }
    }
    if (pos < 0) {
      strncpy(li.key, key, MAX_KEY_LEN);
    }

    li.kvs_type = kvs_type;
    li.nval.blob_len = src_len;
    if (li.nval.blob_size == 0)
      li.nval.blob_size = src_len;

    int res = kvs_write(h, &li, pos);

    res = write(h->fd, src, src_len);
  }
}

unsigned kvs_rw_str(kvshT h, const char *key, void *src_or_dst, unsigned length, bool do_write) {
  if (do_write)
    length = strlen(src_or_dst);
  return kvs_rw_str_or_blob(h, key, src_or_dst, length, do_write, KVS_TYPE_STR);
}

unsigned kvs_rw_blob(kvshT h, const char *key, void *src_or_dst, unsigned length, bool do_write) {
  return kvs_rw_str_or_blob(h, key, src_or_dst, length, do_write, KVS_TYPE_BLOB);
}

void kvs_setup(void) {

}

#ifdef TEST_MODULE


kvs_cbrT forEach_cb(const char *key, kvs_type_t type) {
  printf("foreach: %s, %d\n", key, type);
}

int main() {

  kvshT h = kvs_open("TEST", kvs_WRITE);
  printf("kvs_open: %p\n", h);
  if (!h)
    return -1;
  bool succ = kvs_set_i8(h, "test_i8", 42);
  printf("set_i8: %d\n", succ);
  if (!succ)
    return -1;
  i8 val = kvs_get_i8(h, "test_i8", 33, 0);
  printf("get_i8: %d\n", (int) val);

  {
    char *s = "my_string_value";
    int s_len = strlen(s);
    int res = kvs_rw_str(h, "test_strX", s, s_len, true);
    printf("w_str: %d\n", (int) res);
  }
  {
    char *s = "my_string_value";
    int s_len = strlen(s);
    int res = kvs_rw_str(h, "test_str", s, s_len, true);
    printf("w_str: %d\n", (int) res);
  }

  char buf[32];
  int res = kvs_rw_str(h, "test_str", buf, 32, false);
  printf("w_str: %d, %s\n", (int) res, buf);

  kvs_close(h);
  res = kvs_foreach("TEST", KVS_TYPE_STR, 0, forEach_cb);

}


#endif

/*
 Local Variables:
 compile-command: "gcc -g3 kvs_posix.c -o kvs -I../include/key_value_store" && ./kvs
 js-indent-level: 2
 indent-tabs-mode: nil
 End:
 */
