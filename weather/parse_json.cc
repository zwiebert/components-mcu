#include "weather/weather.hh"
#include "jsmn/jsmn.h"
#include <cstring>
#include <cstdlib>
#include <cassert>

static bool tok_keyIsEqual(const char *json, const jsmntok_t *tok, const char *key) {
  return tok->type == JSMN_STRING // JSON keys are strings
  && strlen(key) == tok->end - tok->start // same length
  && strncmp(json + tok->start, key, tok->end - tok->start) == 0; // same content
}

static bool tok_keyIsEqual(const char *json, const jsmntok_t *tok, const char *key, jsmntype_t kind) {
  if (kind != tok[1].type)
    return false;
  if (!key)
    return true;
  return tok_keyIsEqual(json, tok, key);
}

static int tok_objSize(const jsmntok_t *tok) {
  if (tok->type == JSMN_OBJECT) {
    return tok->size;
  }
  return 0;
}

static char* copy_string(char *buf, size_t buf_size, const char *json, const jsmntok_t *t) {
  size_t str_length = t->end - t->start;
  if (buf_size <= str_length)
    return NULL;
  memcpy(buf, json + t->start, str_length);
  buf[str_length] = '\0';
  return buf;
}

static bool tok_getValue(float &dst, const char *json, const jsmntok_t *tok, const char *key) {
  char buf[8];
  if (tok_keyIsEqual(json, &tok[0], key, JSMN_PRIMITIVE)) {
    if (copy_string(buf, sizeof buf, json, &tok[1]))
      dst = atof(buf);
    return true;
  }
  return false;
}

static bool tok_getValue(unsigned &dst, const char *json, const jsmntok_t *tok, const char *key) {
  char buf[8];
  if (tok_keyIsEqual(json, &tok[0], key, JSMN_PRIMITIVE)) {
    if (copy_string(buf, sizeof buf, json, &tok[1]))
      dst = atoi(buf);
    return true;
  }
  return false;
}

typedef int (*tok_processObj_funT)(weather_data &weather, const char *json, const jsmntok_t *t, unsigned size);


static int tok_processObj_consume(weather_data &weather, const char *json, const jsmntok_t *t, unsigned size) {
  if (tok_keyIsEqual(json, t, nullptr, JSMN_OBJECT)) {
    int i = 2;
    for (int childs = tok_objSize(&t[1]); childs > 0 && i < size; --childs) {
      assert(t[i].type == JSMN_STRING);
      i += tok_processObj_consume(weather, json, t + i, size);
    }
    return i;
  } else if (tok_keyIsEqual(json, t, nullptr, JSMN_ARRAY)) {
    int i = 1; // array members have no key, only values
    for (int childs = tok_objSize(&t[1]); childs > 0 && i < size; --childs) {
      assert(t[i].type == JSMN_STRING);
      i += tok_processObj_consume(weather, json, t + i, size);
    }
    return i;
  } else if (tok_keyIsEqual(json, t, nullptr, JSMN_STRING)) {
    return 2;
  } else if (tok_keyIsEqual(json, t, nullptr, JSMN_PRIMITIVE)) {
    return 2;
  } else {
    return 2;
  }
}

static const tok_processObj_funT tok_processRootChilds_funs[] = { //
    [](weather_data &weather, const char *json, const jsmntok_t *t, unsigned size) -> int {
      if (tok_keyIsEqual(json, t, "main", JSMN_OBJECT)) {
        int i = 2;
        for (int childs = tok_objSize(&t[1]); childs > 0 && i < size; --childs) {
          assert(t[i].type == JSMN_STRING);
          if (tok_getValue(weather.main.humidity, json, t + i, "humidity") || tok_getValue(weather.main.temp, json, t + i, "temp")
              || tok_getValue(weather.main.pressure, json, t + i, "pressure")) {
            i += 2;
          } else {
            i += tok_processObj_consume(weather, json, t + i, size);
          }
        }
        return i;
      }
      return 0;

    },
    [](weather_data &weather, const char *json, const jsmntok_t *t, unsigned size) -> int {
      if (tok_keyIsEqual(json, t, "wind", JSMN_OBJECT)) {
        int i = 2;
        for (int childs = tok_objSize(&t[1]); childs > 0 && i < size; --childs) {
          assert(t[i].type == JSMN_STRING);

          if (tok_getValue(weather.wind.speed, json, t + i, "speed") || tok_getValue(weather.wind.deg, json, t + i, "deg")) {
            i += 2;
          } else {
            i += tok_processObj_consume(weather, json, t + i, size);
          }
        }
        return i;
      }
      return 0;
    },
    [](weather_data &weather, const char *json, const jsmntok_t *t, unsigned size) -> int {
      if (tok_keyIsEqual(json, t, "clouds", JSMN_OBJECT)) {
        int i = 2;
        for (int childs = tok_objSize(&t[1]); childs > 0 && i < size; --childs) {
          assert(t[i].type == JSMN_STRING);

          if (tok_getValue(weather.clouds.all, json, t + i, "all")) {
            i += 2;
          } else {
            i += tok_processObj_consume(weather, json, t + i, size);
          }
        }
        return i;
      }
      return 0;
    },
    tok_processObj_consume };

static inline int tok_processObject(weather_data &weather, const char *json, const jsmntok_t *t, unsigned size) {
  for (auto fun : tok_processRootChilds_funs) {
    if (int result = fun(weather, json, t, size); result > 0)
      return result;
  }
  return 0;
}

bool weather_process_json(const char *json, weather_data &weather) {
  const unsigned JSON_MAX_TOKENS = 100;
  jsmn_parser parser;
  jsmntok_t t[JSON_MAX_TOKENS];
  jsmn_init(&parser);
  int r = jsmn_parse(&parser, json, strlen(json), t, JSON_MAX_TOKENS);
  if (r < 0) {
    return false;
  }
  if (r < 1 || t[0].type != JSMN_OBJECT) {
    return false;
  } else {
    char buf[8];
    for (int i = 1; i < r;) {
      int tokens_processed = tok_processObject(weather, json, t + i, r - i);
      assert(tokens_processed);
      i += tokens_processed;
    }
    return true;
  }
}

