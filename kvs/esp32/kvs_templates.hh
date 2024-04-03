#pragma once

#define CFG_PARTNAME "nvs"

#include "nvs.h"

/// just count matching keys. no callback
template<typename cmp_fun_type, typename cmp_arg_type>
int kvs_foreach(const char *name_space, kvs_type_t type, cmp_fun_type cmp, cmp_arg_type &&cmp_arg) {
  nvs_iterator_t it = NULL;
  esp_err_t res = nvs_entry_find(CFG_PARTNAME, name_space, (nvs_type_t) type, &it);

  int count = 0;
  while (res == ESP_OK) {
    nvs_entry_info_t info;
    nvs_entry_info(it, &info);
    res = nvs_entry_next(&it);

    if (cmp && !cmp(info.key, cmp_arg))
      continue;

    ++count;
  }
  return count;
}

template<typename cmp_fun_type, typename cmp_arg_type, typename cb_fun_type, typename cb_arg_type>
int kvs_foreach(const char *name_space, kvs_type_t type, cmp_fun_type cmp, cmp_arg_type &&cmp_arg, cb_fun_type cb, cb_arg_type &&cb_arg) {
  assert(cb);

  nvs_iterator_t it = NULL;
  esp_err_t res = nvs_entry_find(CFG_PARTNAME, name_space, (nvs_type_t) type, &it);

  int count = 0;
  while (res == ESP_OK) {
    nvs_entry_info_t info;
    nvs_entry_info(it, &info);
    res = nvs_entry_next(&it);

    if (cmp && !cmp(info.key, cmp_arg))
      continue;

    switch (cb(info.key, (kvs_type_t) info.type, cb_arg)) {
    case kvsCb_match:
      ++count;
      break;
    case kvsCb_done:
      ++count;
      return count;
    case kvsCb_noMatch:
      break;
    }

  }
  return count;
}

