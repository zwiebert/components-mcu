#include <weather/weather.hh>
#include <weather/to_json.hh>
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
  struct {
    time_t time = 0;
    uint8_t version = 0;
  } info;
  weather_data_v1 wd;
  char padding[wd_store_size - sizeof info - sizeof wd] = { };
};

static_assert(sizeof(time_t) == 8);
static_assert(sizeof(wd_store::info) == 16);
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

bool Weather::load_past_weather_data() {
  int err = 0;
  char key[64];
  time_t tmin = time(0) - (60 * 60 * 24 * 7); // data not older than one week

  if (auto h = kvs_open(kvs_name, kvs_READ)) {
    wd_store wds;
    for (unsigned wday = 0; wday < 7; ++wday) {
      for (unsigned hour = 0; hour < 24; ++hour) {

        auto n = snprintf(key, sizeof key, kvs_key_fmt, wday, hour);
        assert(n < sizeof key);

        if (kvs_get_blob(h, key, &wds, sizeof wds)) {
          if (tmin <= wds.info.time) { // filter out too old data
            if (wd_current_version == wds.info.version) {
              m_past_wd[wday][hour] = wds.wd;
            }
          }
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

  const wd_store wds = { .info { .time = time(0), .version = wd_current_version }, .wd = wd };

  if (auto h = kvs_open(kvs_name, kvs_WRITE)) {
    if (!kvs_set_blob(h, key, &wds, sizeof wds))
      ++err;
    kvs_commit(h);
    kvs_close(h);
  }

  return !err;
}

int Weather::to_json(char *buf, size_t buf_size, int &obj_ct, int &state, int start_ct) {
  int bi = 0;
  int arr_idx = obj_ct - start_ct;

  if (!(0 <= arr_idx && arr_idx < TOTAL_OBJS))
    return 0; // out of our range (EOF)

  bi += array_to_json_tmpl(buf + bi, buf_size - bi, obj_ct, &m_past_wd[0][0], PAST_WD_OBJS, "past_wdj", start_ct);
  state = arr_idx+1 == TOTAL_OBJS;
  return bi;
}

/////////////dev/////////////////
#include <cstdlib>

template<typename T>
static T rando(T low_limit, T high_limit) {
  return low_limit + static_cast<T>(rand()) / (static_cast<T>(RAND_MAX / (high_limit - low_limit)));
}

static void init_wd_rand(weather_data &wd) {
  wd.main.temp = 273.15 + rando(5.0, 37.0);
  wd.main.humidity = rando(0, 100);
  wd.clouds.all = rando(0, 100);
  wd.wind.speed = rando(0, 85);
}

void Weather::dev_fill_past_wd_randomly() {
  for (unsigned wday = 0; wday < 7; ++wday)
    for (unsigned hour = 0; hour < 24; ++hour) {
      init_wd_rand(m_past_wd[wday][hour]);
    }
}
