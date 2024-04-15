/**
 * \file uout/uout_builder_json.hh
 * \brief  Create JSON output from key/val pairs
 */
#pragma once

#include <stdlib.h>
#include <functional>

/**
 * \brief Build JSON output gradually by calling member functions (with key/value parameters).
 */
class UoutBuilderJson {
public:
  UoutBuilderJson(char *buf, size_t buf_size, class UoutWriter *td = 0) :
      myTd(td), myBuf(buf), myBuf_size(buf_size), myBuf_isMine(false){
  }
  UoutBuilderJson(UoutWriter *td = 0) :
      myTd(td) {
  }
  ~UoutBuilderJson();

public:
  bool open_root_object(const char *id); ///< \brief add root object  {"from":"ID",
  bool open_root_object(); ///< \brief add root object  {
  void close_root_object(); ///< \brief close the root object with },

  int add_object(const char *key); ///< \brief add object "KEY":{
  int add_object(); ///< \brief add object w/o a key (e.g. in arrays):{
  void close_object();  ///< \brief close object with },


  bool put_kv(const char *key, bool val); ///< \brief  Add key value pair "KEY":VAL
  bool put_kv(const char *key, unsigned val); ///< \brief  Add key value pair "KEY":VAL
  bool put_kv(const char *key, int val); ///< \brief  Add key value pair "KEY":VAL
  bool put_kv(const char *key, long unsigned val); ///< \brief  Add key value pair "KEY":VAL
  bool put_kv(const char *key, long int val); ///< \brief  Add key value pair "KEY":VAL
  bool put_kv(const char *key, long long unsigned val); ///< \brief  Add key value pair "KEY":VAL
  bool put_kv(const char *key, long long int val); ///< \brief  Add key value pair "KEY":VAL

  bool put_kv(const char *key, const char *val); ///< \brief  Add key value pair "KEY":"VAL"

  bool add_key_value_pair_s(const char *key, const char *val); ///< \brief  Add key value pair "KEY":"VAL"
  bool add_key_value_pair_d(const char *key, int val); ///< \brief  Add key value pair "KEY":VAL
  bool add_key_value_pair_f(const char *key, float val, int prec = 5); ///< \brief  Add key value pair "KEY":VAL
  bool add_key(const char *key); ///< \brief  Add a single key with colon but no value

  bool add_array(const char *key);  ///< \brief  add array  "KEY":[
  bool add_array(); ///< \brief add array w/o a key (e.g. in arrays):[
  void close_array(); ///< \brief close array with ],
  bool add_value_d(int n);  ///< \brief Add number to array: N,
  bool add_value_s(const char *val); //< \brief Add string to array,
  bool add_value(long int n);  ///< \brief Add number to array: N,


  /**
   * \brief                   a snprintf like function writing JSON content like unamed objects, primitives. (things you could place in an array)
   *                          \ref read_json_from_function
   * \param arr_idx           index of the element we want the function to generate the JSON
   * \param dst        output buffer
   * \param dst_size   output buffer size
   * \return                  return value should be exactly be like snprintf(3)'s return value
   */
using uo_to_json_funT = std::function<int(char *dst, size_t dst_size)>;
  /**
   * \brief                   a snprintf like function writing JSON for an array element used in JSON writer for arrays
   *                          \ref read_json_arr_from_function
   * \param dst        output buffer
   * \param dst_size   output buffer size
   * \param arr_idx           index of the element we want the function to generate the JSON
   * \return                  return value should be exactly be like snprintf(3)'s return value
   */
using uo_to_json_arr_funT = std::function<int(char *dst, size_t dst_size, unsigned arr_idx)>;
  /**
   * \brief                   a snprintf like function writing JSON for an array element used in JSON writer for 2 dimensional arrays
   *                          \ref read_json_arr2_from_function
   * \param dst        output buffer
   * \param dst_size   output buffer size
   * \param arr_lidx          index of the nested array
   * \param arr_ridx          index of the element inside the nested array we want the function to generate the JSON
   * \return                  return value should be exactly be like snprintf(3)'s return value
   */
using uo_to_json_arr2_funT = std::function<int(char *dst, size_t dst_size, unsigned arr_lidx, unsigned arr_ridx)>;

  /**
   * \brief                   let a snprintf like function insert a JSON value, object, array element (anything which could be followed by a comma)
   * \param f                 this function should behave exactly like snprintf(3). We do a second pass based on its return value if buffer was too small.
   * \param required_size     if you know the size of the produced JSON, feel free to provide it here. It saves the second pass. Making it too large is wasteful.
   *                          To increase chunk size use get_a_buffer(), because this parameter will not work for that
   * \return                  success
   */
  bool read_json_from_function(uo_to_json_funT f, size_t required_size = 256);
  /**
   * \brief                   let a snprintf like function write JSON array elements directly into our buffer and add a comma
   * \param f                 this function should behave exactly like snprintf(3). We do a second pass based on its return value if buffer was too small.
   * \param arr_len           number of elements in the array (array length)
   * \param required_size     if you know the size of the produced JSON per element, feel free to provide it here. It saves the second pass. Making it too large is wasteful.
   *                          To increase chunk size use get_a_buffer(), because this parameter will not work for that
   * \return                  success
   */
  bool read_json_arr_from_function(uo_to_json_arr_funT f, unsigned arr_len, size_t required_size = 256);

  /**
   * \brief                   let a snprintf like function write JSON array elements directly into our buffer and add a comma
   * \param f                 this function should behave exactly like snprintf(3). We do a second pass based on its return value if buffer was too small.
   * \param arr_llen          number of nested arrays   arr[arr_llen][]
   * \param arr_rlen          number of array elements   arr[][arr_rlen]
   * \param required_size     if you know the size of the produced JSON per element, feel free to provide it here. It saves the second pass. Making it too large is wasteful.
   *                          To increase chunk size use get_a_buffer(), because this parameter will not work for that
   * \return                  success
   */
  bool read_json_arr2_from_function(uo_to_json_arr2_funT f, unsigned arr_llen,
      unsigned arr_rlen, size_t required_size = 256);

  // copy/cat some json int buf
  bool copy_to_buf(const char *s); ///< \brief Copy some external JSON into this object's buffer (buffer will be overwritten)
  bool cat_to_buf(const char *s); ///< \brief Append some external JSON to this object's buffer

  char* get_json() const; ///< \brief  Get this objects JSON as null terminated string (ownership remains to this object)
  int writeln_json(bool final = true); ///< \brief write this objects JSON as line to its Target descriptor
  int write_json(bool final = true); ///< \brief write this objects JSON to its Target descriptor w/o newline
  int write_some_json(); ///< \brief write buffer to its Target (final=false), but leave the last character in the buffer at index 0.
  void free_buffer(); ///<  Destroy this objects JSON buffer (Optional. Destructor will take care of it)

  bool alloc_bigger_buffer(size_t total_size); ///< Alloc a bigger buffer to avoid too many small chunks in writing. Size is the actual size of the buffer.
  char* get_a_buffer(size_t size); ///< get a buffer of at least size. returns a pointer inside the main buffer at current buf_idx
  bool advance_position(int n); ///< advance buf_idx after writing to buffer from get_a_buffer. n can be negative. returns false if n is out of range

private:
  bool realloc_buffer(size_t buf_size);
  bool not_enough_buffer(const char *key = "", const char *val = nullptr);
  bool buffer_grow(size_t required_free_space = 0);

  class UoutWriter *myTd = 0;
  char *myBuf = 0;
  size_t myBuf_size = 0;
  unsigned myBuf_idx = 0;
  int m_obj_ct = 0;

  bool myBuf_isMine = true;
  bool m_eof = false;
};
