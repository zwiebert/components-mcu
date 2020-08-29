#ifdef __cplusplus
  extern "C" {
#endif
/*
 * status_json.h
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#pragma once

#include <stdbool.h>
#include <stdlib.h>

#ifdef USE_SJ_WRITE
extern int (*sj_write)(const char *src, unsigned len);
#define sj_write_set(write_fun) sj_write = (write_fun)
#else
#define sj_write 0
#define sj_write_set(write_fun)
#endif

bool sj_open_root_object(const char *id);
bool sj_add_object(const char *key);
bool sj_add_key_value_pair_s(const char *key, const char *val);
bool sj_add_key_value_pair_d(const char *key, int val);
bool sj_add_key_value_pair_f(const char *key, float val);

bool sj_add_array(const char *key);
void sj_close_array();
bool sj_add_value_d(int val);

void sj_close_object();
void sj_close_root_object();

// copy/cat some json int buf
bool sj_copy_to_buf(const char *s);
bool sj_cat_to_buf(const char *s);

bool sj_realloc_buffer(size_t buf_size);
char *sj_get_json();
void sj_free_buffer();

#ifdef __cplusplus
  }
#endif
