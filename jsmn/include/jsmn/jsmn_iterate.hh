/**
 * \file     jsmn_iterate.hh
 * \brief    helper class to iterate the JSMN token array and extract data
 * \author   Bert Winkelmann (github.com/zwiebert)
 */

#pragma once

#include "jsmn/jsmn.h"

#include <iterator>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <string>


class JsmnBase {
using pointer = jsmntok_t *;

protected:
JsmnBase(const char *json, jsmntok_t *tok, unsigned tok_max) :
    m_json(json),
    m_tok_heap_alloc(nullptr),
    m_tok(tok),
    m_tok_max(tok_max),
    m_nmb_tok(do_parse(json)) {
}
public:

  JsmnBase(const char *json, unsigned tok_max) :
      m_json(json),
      m_tok_heap_alloc(new jsmntok_t[tok_max]),
      m_tok(m_tok_heap_alloc),
      m_tok_max(tok_max),
      m_nmb_tok(do_parse(json)) {
  }

  ~JsmnBase() {
    delete[] m_tok_heap_alloc;
  }

  operator bool() const {
    return m_json && m_nmb_tok > 0;
  }

  const char* get_json() const {
    return m_json;
  }

  struct Iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = jsmntok_t;
    using pointer = value_type *;
    using reference = value_type &;
    using container_type = JsmnBase;

    Iterator(pointer ptr, container_type &container) :
        m_ptr(ptr), m_container(container) {
    }

  public:
    bool value_equals_null() const {
      const char s[] = "null";
      const unsigned slen = sizeof s -1;

      return m_ptr->type == JSMN_PRIMITIVE
      && slen == m_ptr->end - m_ptr->start // same length
      && strncmp(m_container.get_json() + m_ptr->start, "null", slen) == 0; // same content
    }

    bool value_equals_false() const {
      const char s[] = "false";
      const unsigned slen = sizeof s - 1;

      return m_ptr->type == JSMN_PRIMITIVE
      && slen == m_ptr->end - m_ptr->start // same length
      && strncmp(m_container.get_json() + m_ptr->start, "null", slen) == 0; // same content
    }

    bool value_equals_true() const {
      const char s[] = "true";
      const unsigned slen = sizeof s - 1;;

      return m_ptr->type == JSMN_PRIMITIVE
      && slen == m_ptr->end - m_ptr->start // same length
      && strncmp(m_container.get_json() + m_ptr->start, "null", slen) == 0; // same content
    }


    /**
     * \brief        test if key matches
     * \param  key   key to match
     * \return       false if key does not match or type is not JSMN_STRING
     */
    bool keyIsEqual(const char *key) const {
      return m_ptr->type == JSMN_STRING // JSON keys are strings
      && strlen(key) == m_ptr->end - m_ptr->start // same length
      && strncmp(m_container.get_json() + m_ptr->start, key, m_ptr->end - m_ptr->start) == 0; // same content
    }

    /**
     * \brief           test if both key and value-type matches
     * \param  key      string to match key or nullptr to match any key
     * \param  val_type type of value to match
     * \return          false for mismatch or key type not JSMN_STRING
     */
    bool keyIsEqual(const char *key, jsmntype_t val_type) const {
      if (val_type != m_ptr[1].type)
        return false;
      return !key || keyIsEqual(key);
    }

    /**
     * \brief        Get value
     * \dst          value will be written to dst
     * \return       false if value type is not JSMN_PRIMITIVE
     */
    template<typename T>
    bool getValue(T &dst) const {
      return m_container.get_value(dst, m_ptr);
    }

    /**
     * \brief        Get value of key/value pair if key
     * \param  key   key to match or nullptr to match any key
     * \param  dst   value will be written to dst
     * \return       false if key does not match, or value type is not JSMN_PRIMITIVE
     */
    template<typename T>
    bool getValue(T &dst, const char *key) const {
      return keyIsEqual(key) && m_container.get_value(dst, m_ptr + 1);
    }

  public: // operators
    reference operator*() const {
      return *m_ptr;
    }
    pointer operator->() {
      return m_ptr;
    }

    Iterator& operator+=(int n) {
      m_ptr += n;
      return *this;
    }

    Iterator& operator-=(int n) {
      m_ptr -= n;
      return *this;
    }

    // Prefix increment
    Iterator& operator++() {
      m_ptr++;
      return *this;
    }

    // Postfix increment
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    reference operator[](int idx) {
      return m_ptr[idx];
    }

    operator bool() const {
      return m_ptr >= &m_container.m_tok[0] && &m_container.m_tok[0] + m_container.m_nmb_tok > m_ptr;
    }

    friend bool operator==(const Iterator &a, const Iterator &b) {
      return a.m_ptr == b.m_ptr;
    }

    friend bool operator!=(const Iterator &a, const Iterator &b) {
      return a.m_ptr != b.m_ptr;
    }

    friend bool operator<(const Iterator &a, const Iterator &b) {
      return a.m_ptr < b.m_ptr;
    }

    friend bool operator>(const Iterator &a, const Iterator &b) {
      return a.m_ptr > b.m_ptr;
    }


  private:
    pointer m_ptr;
    container_type &m_container;
  };

  Iterator begin() {
    return Iterator(&m_tok[0], *this);
  }
  Iterator end() {
    return Iterator(&m_tok[m_nmb_tok], *this);
  }

  static bool skip_value(Iterator &it) {
    if (!it)
      return false;

    if (it->type == JSMN_OBJECT) {
      auto count = it->size;
      for (++it; count > 0 && it; --count) {

        assert(it->type == JSMN_STRING);
        skip_key_and_value(it);
      }
      return true;
    }

    if (it->type == JSMN_ARRAY) {
      auto count = it->size;
      for (++it; count > 0 && it; --count) {
        skip_value(it);
      }
      return true;
    }

    ++it;
    return true;
  }

  static bool skip_key_and_value(Iterator &it) {
    ++it;
    if (!it)
      return false;
    return skip_value(it);
  }

private:
  int do_parse(const char *json) {
    if (!json)
      return false;

    jsmn_parser parser;

    jsmn_init(&parser);
    int r = jsmn_parse(&parser, json, strlen(json), m_tok, m_tok_max);
    if (r < 0) {
      return 0;
    }
    if (r < 1 || m_tok[0].type != JSMN_OBJECT) {
      return 0;
    }
    return r;
  }

  char* copy_string(char *buf, size_t buf_size, pointer ptr) const {
    size_t str_length = ptr->end - ptr->start;
    if (buf_size <= str_length)
      return NULL;
    memcpy(buf, m_json + ptr->start, str_length);
    buf[str_length] = '\0';
    return buf;
  }

  bool get_value(float &dst, pointer ptr) const {
    char buf[32];
    if (ptr->type == JSMN_PRIMITIVE && copy_string(buf, sizeof buf, ptr)) {
      dst = atof(buf);
      return true;
    }
    return false;
  }

  bool get_value(std::string &dst, pointer ptr) const {
    if (ptr->type == JSMN_STRING) {
      size_t str_length = ptr->end - ptr->start;
      dst = std::string(ptr->start, str_length);
      return true;
    }
    return false;
  }

  template<typename T>
  bool get_value(T &dst, pointer ptr) const {
    char buf[32];
    if (ptr->type == JSMN_PRIMITIVE && copy_string(buf, sizeof buf, ptr)) {
      dst = static_cast<T>(atol(buf));
      return true;
    }
    return false;
  }

  template<size_t size>
  bool get_value(char (&dst)[size], pointer ptr) const {
    if (ptr->type == JSMN_STRING && copy_string(dst, size, ptr)) {
      return true;
    }
    return false;
  }

private:
  const char *m_json;
  jsmntok_t *m_tok_heap_alloc;
  jsmntok_t *m_tok;
  unsigned m_tok_max;
  unsigned m_nmb_tok;

};

template<size_t JSON_MAX_TOKENS>
class Jsmn : public JsmnBase {
public:
  Jsmn(const char *json): JsmnBase(json, &m_tok_arr[0], JSON_MAX_TOKENS) {
  }
private:
  jsmntok_t m_tok_arr[JSON_MAX_TOKENS];
};

