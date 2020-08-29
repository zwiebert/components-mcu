#ifdef __cplusplus
  extern "C" {
#endif
/*
 * kvs_wrapper.h
 *
 *  Created on: 24.02.2020
 *      Author: bertw
 */

#pragma once

#include <stdint.h>
#include "stdbool.h"

enum { kvs_READ=1, kvs_WRITE=2, kvs_READ_WRITE=3 };

struct kvs_handle;
typedef struct kvs_handle *kvshT;


kvshT kvs_open(const char *name, unsigned flags);
bool kvs_commit(kvshT handle);
void kvs_close(kvshT handle);

bool kvs_erase_key(kvshT handle, const char *key);

unsigned kvs_rw_blob(kvshT handle, const char *key, void *src_or_dst, unsigned length, bool write);
unsigned kvs_rw_str(kvshT handle, const char *key, char *src_or_dst, unsigned length, bool write);


bool kvs_set_i8(kvshT handle, const char *key, int8_t val);
int8_t kvs_get_i8(kvshT handle, const char *key, int8_t default_val, bool *res_success);
bool kvs_set_u8(kvshT handle, const char *key, uint8_t val);
uint8_t kvs_get_u8(kvshT handle, const char *key, uint8_t default_val, bool *res_success);

bool kvs_set_i16(kvshT handle, const char *key, int16_t val);
int16_t kvs_get_i16(kvshT handle, const char *key, int16_t default_val, bool *res_success);
bool kvs_set_u16(kvshT handle, const char *key, uint16_t val);
uint16_t kvs_get_u16(kvshT handle, const char *key, uint16_t default_val, bool *res_success);

bool kvs_set_i32(kvshT handle, const char *key, int32_t val);
int32_t kvs_get_i32(kvshT handle, const char *key, int32_t default_val, bool *res_success);
bool kvs_set_u32(kvshT handle, const char *key, uint32_t val);
uint32_t kvs_get_u32(kvshT handle, const char *key, uint32_t default_val, bool *res_success);

bool kvs_set_i64(kvshT handle, const char *key, int64_t val);
int64_t kvs_get_i64(kvshT handle, const char *key, int64_t default_val, bool *res_success);
bool kvs_set_u64(kvshT handle, const char *key, uint64_t val);
uint64_t kvs_get_u64(kvshT handle, const char *key, uint64_t default_val, bool *res_success);


typedef enum {
    KVS_TYPE_u8    = 0x01,  /*!< Type uint8_t */
    KVS_TYPE_i8    = 0x11,  /*!< Type int8_t */
    KVS_TYPE_u16   = 0x02,  /*!< Type uint16_t */
    KVS_TYPE_i16   = 0x12,  /*!< Type int16_t */
    KVS_TYPE_u32   = 0x04,  /*!< Type uint32_t */
    KVS_TYPE_i32   = 0x14,  /*!< Type int32_t */
    KVS_TYPE_u64   = 0x08,  /*!< Type uint64_t */
    KVS_TYPE_i64   = 0x18,  /*!< Type int64_t */
    KVS_TYPE_STR   = 0x21,  /*!< Type string */
    KVS_TYPE_BLOB  = 0x42,  /*!< Type blob */
    KVS_TYPE_ANY   = 0xff   /*!< Must be last */
} kvs_type_t;

typedef enum {
  kvsCb_noMatch, kvsCb_match, kvsCb_done
} kvs_cbrT;

typedef kvs_cbrT (*kvs_foreach_cbT)(const char *key, kvs_type_t type, void *args);

int kvs_foreach(const char *name_space, kvs_type_t, const char*key_match, kvs_foreach_cbT, void *args);


void kvs_setup(void);


#ifdef __cplusplus
  }
#endif
