#define ERR_CORRUPT_FILE -42
#define MAX_KEY_LEN 14
#define COOKIE 0xbeef

struct kvs_handle {
  char *buf;
  int fd;
  unsigned file_size :14;
  unsigned in_use :1;
};

struct line_info {
  uint16_t magic;
  uint8_t kvs_type :8;
  uint8_t xx;
  union {
    uint8_t val_u8;
    int8_t val_i8;
    uint16_t val_u16;
    int16_t val_i16;
    uint32_t val_u32;
    int32_t val_i32;
    int64_t val_i64;
    uint64_t val_u64;
    struct {
      uint16_t blob_len, blob_size;
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

int kvs_find_next(kvshT h, struct line_info *li, int pos, const char *key, kvs_type_t kvs_type);
bool kvs_checkMagicCookie(struct line_info *lip, int pos);

/// just count matching keys. no callback
template<typename cmp_fun_type, typename cmp_arg_type>
int kvs_foreach(const char *name_space, kvs_type_t type, cmp_fun_type cmp, cmp_arg_type &&cmp_arg) {

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

      if (cmp && !cmp(li.key, cmp_arg))
        continue;

      ++count;
    }
    goto succ;
  }

  succ: if (h)
    kvs_close(h);
  return count;
}

template<typename cmp_fun_type, typename cmp_arg_type, typename cb_fun_type, typename cb_arg_type>
int kvs_foreach(const char *name_space, kvs_type_t type, cmp_fun_type cmp, cmp_arg_type &&cmp_arg, cb_fun_type cb, cb_arg_type &&cb_arg) {

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

      if (cmp && !cmp(li.key, cmp_arg))
        continue;

      if (cb) {
        switch (cb(li.key, (kvs_type_t) li.kvs_type, cb_arg)) {
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

