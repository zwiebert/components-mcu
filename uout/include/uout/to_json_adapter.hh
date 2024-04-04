#pragma once

#include <cstdio>
#include <cstring>
#include <uout/uout_builder_json.hh>
#include <debug/log.h>

/**
 * \brief            template to make an sprintf based obj.to_json(dst, dst_size) to integrate into uout
 * \tparam C         A class which contains a to_json member function. it should return the bytes written, or a negative numer of the requested buffer size
 * \param sj         JSON builder
 * \param obj        object of class C
 * \param buf_size   requested buffer size for the first pass. If it fails we do a second pass using the returned size requirement of obj.to_json
 * \return           true for success
 */
template<class C>
bool uo_to_json(UoutBuilderJson &sj, const C &obj) {
  size_t buf_size = 128;

  // do a second pass with bigger buffer, if was too small at first pass
  for (int pass = 0, n = -1; n < 0 && pass < 2; ++pass) {

    auto buf = sj.get_a_buffer(buf_size + 1); //one extra byte for trailing comma
    if (!buf)
      return false;

    // if buffer was too small, the return value of obj.to_json
    // should be negative and contain the required size
    n = obj.to_json(buf, buf_size);

    if (n < 0) {
      buf_size = -n;
      continue;
    }
#ifdef UOUT_DEBUG
    db_loge(__func__, "add to sj buffer (buf_size=%u, n=%d): <%.*s>", buf_size, n, n, buf);
#endif
    buf[n++] = ',';
    return sj.advance_position(n);
  }

  return false;
}

/**
 * \brief            template to make an array of sprintf based objects with obj.to_json(dst, dst_size) integrate into uout
 * \tparam C         type of the array elements
 * \param sj         JSON builder (and optional writer)
 * \param arr        a pointer the first element of a 1 dimensional array  arr[arr_len]
 * \param arr_len    array length
 * \return           success
 */
template<class C>
bool uo_to_json(UoutBuilderJson &sj, const C *arr, size_t arr_len) {
  if (sj.add_array()) {
    for (int i = 0; i < arr_len; ++i) {
      uo_to_json(sj, arr[i]);
    }
    sj.close_array();
    return true;
  }
  return false;
}

/**
 * \brief            template to make a 2-dimension array of sprintf based objects with obj.to_json(dst, dst_size) integrate into uout
 * \tparam C         type of the array elements
 * \param sj         JSON builder (and optional writer)
 * \param arr        a pointer the first element of a 2 dimensional array  arr[arr_llen][arr_rlen]
 * \param arr_llen   left hand length of the array declaration (outer array)
 * \param arr_rlen   right hand length of the array declaration (inner arrays)
 * \return           success
 */
template<class C>
bool uo_to_json(UoutBuilderJson &sj, const C *arr, size_t arr_llen, size_t arr_rlen) {
  if (sj.add_array()) {
    for (int li = 0; li < arr_llen; ++li) {
      if (sj.add_array()) {
        for (int ri = 0; ri < arr_rlen; ++ri) {
          uo_to_json(sj, arr[li + ri]);
        }
        sj.close_array();
      }
    }
    sj.close_array();
    return true;
  }
  return false;
}

