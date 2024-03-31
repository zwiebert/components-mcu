/**
 * \file config_kvs/config.h
 */
#pragma once


#include <utils_misc/int_types.h>
#include "stdbool.h"

bool config_save_item_s(const char *key, const char *val);
bool config_save_item_b(const char *key, const void *val, unsigned size);
bool config_save_item_u32(const char *key, const char *val, unsigned base = 10);
bool config_save_item_i8(const char *key, const char *val);
bool config_save_item_f(const char *key, const char *val);
bool config_save_item_n_u32(const char *key, uint32_t val);
bool config_save_item_n_i8(const char *key, int8_t val);
bool config_save_item_n_f(const char *key, float val);

const void* config_read_item_b(const char *key, void *d, unsigned d_size, const void *def);
const char* config_read_item_s(const char *key, char *d, unsigned d_size, const char *def);
uint32_t config_read_item_u32(const char *key, uint32_t def);
int8_t config_read_item_i8(const char *key, int8_t def);
float config_read_item_f(const char *key, float def);






