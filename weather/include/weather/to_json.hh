#pragma once

#include <cstdio>
#include <cstring>
#include <uout/status_json.hh>


/**
 * \brief            template to forward to obj.to_json(dst, dst_size)
 * \tparam C         A class which contains a to_json member function. it should return the bytes written, or a negative numer of the requested buffer size
 * \param sj         JSON builder
 * \param obj        object of class C
 * \param buf_size   requested buffer size for the first pass. If it fails we do a second pass using the returned size requirement of obj.to_json
 * \return           true for success
 */
template<class C>
bool to_json(UoutBuilderJson &sj, const C &obj, size_t buf_size = 128) {

  // do a second pass with bigger buffer, if was too small at first pass
  for (int pass = 0, n = -1; n < 0 && pass < 2; ++pass) {

    auto buf = sj.get_a_buffer(buf_size);
    if (!buf)
      return false;

    // if buffer was too small, the return value of obj.to_json
    // should be negative and contain the required size
    n = obj.to_json(buf, buf_size);

    if (n < 0) {
      buf_size = -n;
      continue;
    }

    return sj.advance_position(n);
  }

  return false;
}

/**
 * \brief Read array into buffer as JSON with chunk support
 *
 * \tparam T              type of parameter \ref arr
 * \param dst             buffer where JSON is writte to
 * \param dst_size        buffer size
 * \param obj_ct          this reference counts the total number of already written objects. used to continue each chunk at the right position
 * \param arr             the array currently processed
 * \param arr_size        array size
 * \param key             the key to name the array in the JSON output (key:[...])
 * \param start_ct        as long as obj_ct is less than this parameter, do nothing
 * \return                number of objects written to dst
 */
#ifndef TO_JSON_FLAG_EXISTS
#define TO_JSON_FLAG_EXISTS true
#endif
template<typename T>
int array_to_json_tmpl(char *dst, size_t dst_size, int &obj_ct, T *arr, size_t arr_size, const char *key, int start_ct = 0) {
  const int leave_space = 150;
  int arr_idx = obj_ct - start_ct;
  const int old_arr_idx = arr_idx;
  int bi = 0;
  int key_len = 0;

  if (!(0 <= arr_idx && arr_idx < arr_size))
    return 0; // out of our range, maybe interpreted as EOF by caller
  if (!(leave_space < dst_size))
    return 0;

  if (arr_idx == 0) {
    bi += key_len = snprintf(dst + bi, dst_size - bi, R"("%s":)", key);
  }

  for (; leave_space < (dst_size - bi) && arr_idx < arr_size; ++arr_idx) {
    dst[bi++] = arr_idx == 0 ? '[' : ',';
    const auto &v = arr[arr_idx];

    if (TO_JSON_FLAG_EXISTS) {
      if (int br = v.to_json(dst + bi, dst_size - bi); br)
        bi += br;
      else {
        --bi;  // remove last comma (or bracket)
        goto done;
      }
    } else {
      memcpy(&dst[bi], "null", 4), (bi += 4);
    }

    if (arr_idx + 1 == arr_size) {
      dst[bi++] = ']';
      dst[bi++] = ',';
    }
  }

  done: //
  auto objs_done = arr_idx - old_arr_idx;
  if (!objs_done && key_len)
    bi -= key_len;

  obj_ct += objs_done;
  return bi;
}
