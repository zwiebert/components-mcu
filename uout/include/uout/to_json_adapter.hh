#pragma once

#include <uout/uout_builder_json.hh>
#include <debug/log.h>
#include <cstdio>
#include <cstring>
#include <functional>

/**
 * \brief            template to make an sprintf based obj.to_json(dst, dst_size) to integrate into uout
 * \tparam C         A class which contains a to_json member function. it should return the bytes written, or a negative numer of the requested buffer size
 * \param sj         JSON builder
 * \param obj        object of class C
 * \param buf_size   requested buffer size for the first pass. If it fails we do a second pass using the returned size requirement of obj.to_json
 * \return           true for success
 */
template<class C>
bool uo_elem_to_json(UoutBuilderJson &sj, const C &obj, size_t buf_size = 256) {

  // pass-0: try provide buffer size
  // pass-1: try size returned by JSON generator function to_json()
  for (int pass = 0, n = -1; n < 0 && pass < 2; ++pass) {

    if (n = sj.read_json_from_function(std::bind(&C::to_json, &obj, std::placeholders::_1, std::placeholders::_2), buf_size); n > 0)
      return true;

    if (n == 0)
      return false;

    buf_size = -n;
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
bool uo_arr_to_json(UoutBuilderJson &sj, const C *arr, size_t arr_len, size_t buf_size = 256) {
  if (sj.add_array()) {
    for (int i = 0; i < arr_len; ++i) {
      uo_elem_to_json<C>(sj, arr[i], buf_size);
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
bool uo_arr2_to_json(UoutBuilderJson &sj, const C *arr, size_t arr_llen, size_t arr_rlen, size_t buf_size = 256) {
  if (sj.add_array()) {
    for (int li = 0; li < arr_llen; ++li) {
      if (sj.add_array()) {
        for (int ri = 0; ri < arr_rlen; ++ri) {
          uo_elem_to_json(sj, arr[li + ri], buf_size);
        }
        sj.close_array();
      }
    }
    sj.close_array();
    return true;
  }
  return false;
}

