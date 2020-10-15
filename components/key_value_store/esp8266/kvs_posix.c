/*
 * kvs_posix.c
 *
 *  Created on: 30.03.2020
 *      Author: bertw
 */

#ifndef TEST_HOST
#include "app_config/proj_app_cfg.h"
#endif
#include "key_value_store/kvs_wrapper.h"
#include "utils_misc/int_types.h"
#include "utils_misc/int_macros.h"
#include "debug/dbg.h"

#ifdef HOST_TESTING
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#define io_printf printf
#endif

#include "storage/esp8266/spiffs_posix.h"



#define D(x)
#define DT(x)

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
  u8 xx;
  union {
    u8 val_u8;
    i8 val_i8;
    u16 val_u16;
    i16 val_i16;
    u32 val_u32;
    i32 val_i32;
    struct {
    u16 blob_len,  blob_size;
    } len;
  } nval;
  char key[MAX_KEY_LEN + 1];

#define li_size(li) (sizeof(*li) + ((li->kvs_type & 0x60) ? li->nval.len.blob_size : 0))
#define li_typeInt(li) (0 == ((li)->kvs_type & 0x60) && ((li)->kvs_type & 0x07))
#define li_typeBs(li) (!li_typeInt(li))
#define li_isUsed(li) (0 == ((li)->kvs_type & 0x80) && ((li)->kvs_type != 0))
#define li_matchKey(li,key) (li_isUsed(li) && strncmp(li->key, key, MAX_KEY_LEN) == 0)
};

#define HANDLE_COUNT 4
static struct kvs_handle handles[HANDLE_COUNT];

bool kvs_checkMagicCookie(struct line_info *lip, int pos) {

  if (lip->magic == COOKIE)
    return true;

  D(printf("error: corrupt file (bad magic cookie)\n")); // TODO: delete corrupt file

#ifdef HOST_TESTING
  printf("bad_cookie=%04x at pos=%d\n", lip->magic, pos);
  fflush(stdout);
  abort();
#endif
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

static bool kvs_read(kvshT h, struct line_info *li, int pos) {
  if (pos >= 0)
    if (lseek(h->fd, pos, SEEK_SET) != pos)
      return false;

  return read(h->fd, li, sizeof *li) == sizeof *li && kvs_checkMagicCookie(li, pos);
}

int kvs_find_next(kvshT h, struct line_info *li, int pos, const char *key, kvs_type_t kvs_type) {
  DT(printf("%s:(%p, %p, %d, %s, %d)\n", __func__, h, li, pos, key, kvs_type));
  DT(printf("line_info: kvs_type=0x%02x, key=%s 0x%x\n", li->kvs_type, li->key, li->nval.val_u32));
  int i;

  if (li->kvs_type)
    pos += li_size(li);

  for (i = 0; kvs_read(h, li, pos); ++i) {
    bool found_key = key && li_matchKey(li, key);
    bool found_kvs_type = (li->kvs_type == kvs_type);

    if ((key && !found_key) || (kvs_type != KVS_TYPE_ANY && !found_kvs_type)) {
      pos += li_size(li);
      continue;
    }
    DT(printf("return: %d\n", pos));
    return pos;
  }

   return -1;
}

int kvs_write(kvshT h, struct line_info *li, int pos) {
  DT(printf("%s:(%p, %p, %d)\n", __func__, h, li, pos));
  DT(printf("line_info: kvs_type=0x%02x, key=%s 0x%x (len=%d, size=%d)\n", li->kvs_type, li->key, li->nval.val_u32, li->nval.len.blob_len, li->nval.len.blob_size));
  assert(li_typeInt(li) || li->nval.len.blob_size >= li->nval.len.blob_len);

  if (pos >= 0) {
    if (lseek(h->fd, pos, SEEK_SET) != pos)
      return -1;
  } else {
    pos = lseek(h->fd, 0, SEEK_END);
  }

  unsigned size = sizeof *li;

  if (write(h->fd, li, size) != size)
    return -1;

  pos += size;
  return pos;

}

static void kvs_deleteNode(kvshT h, int pos) {
  //SET_BIT(lip->kvs_type, 7);
  struct line_info li, *lip = &li;

  if (lseek(h->fd, pos, SEEK_SET) != pos)
    goto err;
  read(h->fd, lip, sizeof *lip);
  SET_BIT(lip->kvs_type, 7);
  if (lseek(h->fd, pos, SEEK_SET) != pos)
    goto err;
  kvs_write(h, lip, pos);

  err: return;
}

bool kvs_commit(kvshT handle) {
  (void)handle;
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
    kvs_write(h, &li, pos);
    pos = -1;
    return true;
  }
  return false;
}

int kvs_foreach(const char *name_space, kvs_type_t type, const char *key_match, kvs_foreach_cbT cb, void *args) {
  int pos = 0;
  kvshT h = 0;
  int count = 0;

  if ((h = kvs_open(name_space, kvs_READ))) {
    struct line_info li = { COOKIE };
    while ((pos = kvs_find_next(h, &li, pos, 0, type)) >= 0) {
      if (!li_isUsed(&li))
        continue;

      if (!kvs_checkMagicCookie(&li, pos)) {
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
        switch (cb(li.key, li.kvs_type, args)) {
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

static int find_key_int_w(kvshT h, struct line_info *lip, const char *key) {
#define li (*lip)
  int pos = 0;
  bool ignore_key = false;
  int best_match_pos = -1;

  for (pos = 0; (pos = kvs_find_next(h, lip, pos, 0, KVS_TYPE_ANY)) >= 0;) {
    bool found_key = key && li_matchKey(lip, key);
    bool wrong_type = !li_typeInt(lip);
    bool unused_node = !li_isUsed(lip);

    if (found_key) {
      if (wrong_type) {
        kvs_deleteNode(h,pos);
        ignore_key = true;
        continue;
      }
      return pos;
    }

    if (best_match_pos >= 0)
      continue;

    if (!unused_node)
      continue;
    if (wrong_type)
      continue;

    if (ignore_key)
      return pos;

    best_match_pos = pos;
  }
#undef li
 // li_init(lip);
  return best_match_pos;
}

static int find_key_blob_w(kvshT h, const char *key, unsigned req_size, int *end_pos_ptr) {
  struct line_info li = { .magic=COOKIE }, *lip=&li;

  int pos = 0;
  bool ignore_key = false;
  int best_match_pos = -1;
  int best_match_wasted = 1000;
  int best_match_end_pos = -1;

  for (pos = 0; (pos = kvs_find_next(h, lip, pos, 0, KVS_TYPE_ANY)) >= 0;) {
    bool found_key = key && li_matchKey(lip, key);
    bool wrong_type = li_typeInt(lip);
    bool unused_node = !li_isUsed(lip);
    int wasted = ((int) li.nval.len.blob_size - (int) req_size);
    int end_pos = pos + li_size(lip);

    if (found_key) {
      if (!wrong_type && wasted == 0) {
        if (end_pos_ptr)
          *end_pos_ptr = end_pos;
        return pos;
      }
      kvs_deleteNode(h, pos);
      ignore_key = true;
      unused_node = true;
    }
    if (wrong_type)
      continue;
    if (!unused_node)
      continue;
    if (wasted < 0)
      continue;

    if (wasted == 0 && ignore_key) {
      if (end_pos_ptr)
        *end_pos_ptr = end_pos;
      return pos;
    }
    if (best_match_wasted < wasted)
      continue;

    best_match_wasted = wasted;
    best_match_pos = pos;
    best_match_end_pos = end_pos;
  }

  if (end_pos_ptr)
    *end_pos_ptr = best_match_end_pos;
  return best_match_pos;
}

#define SET_DT_FUN(VAL_T) bool kvs_set_##VAL_T(kvshT h, const char *key, VAL_T val) { \
  struct line_info li = { COOKIE }; \
  int pos = find_key_int_w(h, &li, key); \
  li = (struct line_info){.magic = COOKIE, .kvs_type = KVS_TYPE_##VAL_T, .nval.val_##VAL_T = val, }; \
  STRLCPY(li.key, key, MAX_KEY_LEN+1); \
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

static unsigned kvs_rw_str_or_blob(kvshT h, const char *key, void *src_or_dst, size_t length, bool do_write, kvs_type_t kvs_type) {


  if (!do_write) {
    struct line_info li = { COOKIE };
    char *dst = src_or_dst;
    int pos = kvs_find_next(h, &li, 0, key, kvs_type);

    if (pos < 0)
      return 0;

    unsigned len = li.nval.len.blob_len;
    if (len > length)
      len = length;

    if (read(h->fd, dst, len) != len) {
      return 0;
    }
    return len;
  }

  if (do_write) {
    const char *src = src_or_dst;
    int end_pos;

    int pos = find_key_blob_w(h, key, length, &end_pos);
    bool found_existing = (pos >= 0);
    size_t size = found_existing ? (end_pos - pos - sizeof(struct line_info)) : length;
    if (found_existing) {
      if (size < length) {
        kvs_deleteNode(h, pos);
        pos = -1;
        found_existing = false;
      }
    }
    struct line_info li = { .magic = COOKIE, .kvs_type = kvs_type, .nval.len.blob_len = length, .nval.len.blob_size = size };
    STRLCPY(li.key, key, MAX_KEY_LEN+1);

    int res = kvs_write(h, &li, pos);
    write(h->fd, src, length);
    return res;
  }
  return -1;
}

unsigned kvs_rw_str(kvshT h, const char *key, char *src_or_dst, unsigned length, bool do_write) {
  if (do_write) {
    length = strlen(src_or_dst);
    D(io_printf("key=<%s> val=<%s> len=<%d>\n", key, (char*)src_or_dst, length));
  } else {

  }
  unsigned res = kvs_rw_str_or_blob(h, key, src_or_dst, length, do_write, KVS_TYPE_STR);
  if (!do_write && res > 0) {
    src_or_dst[res] = '\0';
  }
  return res;
}

unsigned kvs_rw_blob(kvshT h, const char *key, void *src_or_dst, unsigned length, bool do_write) {
  return kvs_rw_str_or_blob(h, key, src_or_dst, length, do_write, KVS_TYPE_BLOB);
}

void kvs_setup(void) {

}


/*
 Local Variables:
 compile-command: "gcc -g3 kvs_posix.c -o kvs -I../include/key_value_store" && ./kvs
 js-indent-level: 2
 indent-tabs-mode: nil
 End:
 */
