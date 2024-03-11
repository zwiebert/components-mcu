#include <weather/weather.hh>
#include <key_value_store/kvs_wrapper.h>
#include <cassert>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

static constexpr char kvs_name[] = "kvsWeather";
static constexpr char kvs_key_fmt[] = "pastwd%d%d";

static const size_t wd_store_size = 64;
static constexpr unsigned wd_current_version = 1;
using weather_data_v1 = weather_data;

struct wd_store {
  unsigned version = wd_current_version;
  weather_data_v1 wd;
  char padding[wd_store_size - sizeof version - sizeof wd] = { };
};

static_assert(sizeof(wd_store) == wd_store_size);

Weather::wday_hour Weather::get_wday_hour() {
  Weather::wday_hour result;
  auto now = time(nullptr);
  struct tm *t = localtime(&now);
  if (t) {
    result.wday = t->tm_wday;
    result.hour = t->tm_hour;
  }
  return result;
}

bool Weather::set_weather_provider(Weather_Provider *wp) {
  m_wp = wp;
  return true;
}
const weather_data& Weather::get_past_weather_data(unsigned wday, unsigned hour) {
  assert(wday < 7 && hour < 24);
  return m_past_wd[wday][hour];
}

bool Weather::load_past_weather_data() {
  int err = 0;
  char key[64];

  if (auto h = kvs_open(kvs_name, kvs_READ)) {
    wd_store wds;
    for (unsigned wday = 0; wday < 7; ++wday) {
      for (unsigned hour = 0; hour < 24; ++hour) {

        auto n = snprintf(key, sizeof key, kvs_key_fmt, wday, hour);
        assert(n < sizeof key);

        if (kvs_get_blob(h, key, &wds, sizeof wds)) {
          m_past_wd[wday][hour] = wds.wd;
        }

      }
    }
    kvs_close(h);
  }
  return !err;
}

bool Weather::fetch_and_store_weather_data() {
  int err = 0;
  if (!m_wp)
    return false;
  auto tdh = get_wday_hour();
  //Todo get current wday, hour
  auto &wd = m_past_wd[tdh.wday][tdh.hour];

  if (!m_wp->fetch_weather_data(wd))
    return false;

  char key[64];
  auto n = snprintf(key, sizeof key, kvs_key_fmt, tdh.wday, tdh.hour);
  assert(n < sizeof key);

  const wd_store wds = { .wd = wd };

  if (auto h = kvs_open(kvs_name, kvs_WRITE)) {
    if (!kvs_set_blob(h, key, &wds, sizeof wds))
      ++err;
    kvs_close(h);
  }

  return !err;
}

