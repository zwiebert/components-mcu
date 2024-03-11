#include "include/weather/weather_provider_owm.hh"
#include "jsmn/jsmn_iterate.hh"
#include "net/http_client.h"

#include <cstring>
#include <cstdlib>
#include <cassert>
#include <malloc.h>

using jp = Jsmn<100>;
using jpit = jp::Iterator;
using tok_processObj_funT = bool (*)(weather_data &weather, jpit &it);

static const tok_processObj_funT tok_processRootChilds_funs[] = { //

    [](weather_data &weather, jpit &it) -> bool { // Process object: main
      if (it.keyIsEqual("main", JSMN_OBJECT)) {
        auto count = it[1].size;
        for (it += 2; count > 0 && it; --count) {
          assert(it->type == JSMN_STRING);
          if (it.getValue(weather.main.humidity, "humidity") || it.getValue(weather.main.temp, "temp") || it.getValue(weather.main.pressure, "pressure")) {
            it += 2;
          } else {
            jp::skip_key_and_value(it);
          }
        }
        return true;
      }
      return false;

    },

    [](weather_data &weather, jpit &it) -> bool { // Process object: wind
      if (it.keyIsEqual("wind", JSMN_OBJECT)) {
        auto count = it[1].size;
        for (it += 2; count > 0 && it; --count) {
          assert(it->type == JSMN_STRING);

          if (it.getValue(weather.wind.speed, "speed") || it.getValue(weather.wind.deg, "deg")) {
            it += 2;
          } else {
            jp::skip_key_and_value(it);
          }
        }
        return true;
      }
      return false;
    },

    [](weather_data &weather, jpit &it) -> bool {   // Process object: clouds
      if (it.keyIsEqual("clouds", JSMN_OBJECT)) {

        auto count = it[1].size;
        for (it += 2; count > 0 && it; --count) {
          assert(it->type == JSMN_STRING);

          if (it.getValue(weather.clouds.all, "all")) {
            it += 2;
          } else {
            jp::skip_key_and_value(it);
          }
        }
        return true;
      }
      return false;
    },

    [](weather_data &weather, jpit &it) -> bool { // Throw away unwanted objects
      return jp::skip_key_and_value(it);
    } };

bool weather_process_json(const char *json, weather_data &weather) {
  auto jsmn = jp(json);

  if (!jsmn)
    return false;

  auto it = jsmn.begin();
  if (it->type == JSMN_OBJECT) { // root object
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      assert(it->type == JSMN_STRING);

      for (auto fun : tok_processRootChilds_funs) {
        if (fun(weather, it))
          break;
      }
    }
    return true;
  }

  return false;
}

bool Weather_Provider_Owm::fetch_owm_data(weather_data &data, const char *url)  {
    if (!url)
      return false;

    const size_t buf_size = 750; // current content_length is 471
    char buf[buf_size];

    if (httpClient_getToBuffer(url, buf, buf_size))
      if (weather_process_json(buf, data)) {
        return true;
      }

    return false;
  }



const char* Weather_Provider_Owm::get_url() const {
#ifdef CONFIG_OPENWEATHERMAP_URL_STRING
 if (!m_url)
   return CONFIG_OPENWEATHERMAP_URL_STRING;
#endif
  return m_url;
}

bool Weather_Provider_Owm::set_url(const char *url) {
  int err = 0;

  // copy url to global
  if (url) {
    if (void *p = realloc(m_url, strlen(url) + 1); p) {
      m_url = strcpy((char*) p, url);
    } else {
      ++err; // out of memory
      url = nullptr; // force deletion of m_url
    }
  }

// clear global url if requested or out of memory
  if (!url) {
    free(m_url);
    m_url = nullptr;
  }

  return !err;
}

Weather_Provider_Owm::~Weather_Provider_Owm() {
   set_url(nullptr);
}
