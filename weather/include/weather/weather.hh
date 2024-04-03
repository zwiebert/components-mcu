#pragma once
#include "weather_provider.hh"
#include "weather_data.hh"
#include <cassert>

class Weather {
public:
  Weather(Weather_Provider *wp=nullptr): m_wp(wp) {
  }

public:
  bool set_weather_provider(Weather_Provider *wp); ///< provider to fetch data from
public:
  /**
   * \brief            Get past weather data for given point in time
   * \param wday       day of the week (sun=0...sat=6)
   * \param hour       hour of day (0...23)
   * \return           Filled in weather_data object (success) -or- empty object (no data was available)
   */
  const weather_data& get_past_weather_data(unsigned wday, unsigned hour) const {
    assert(wday < 7 && hour < 24);
    return m_past_wd[wday][hour];
  }
public:
  bool fetch_and_store_weather_data(); ///< get weather data and store it persistently
  bool load_past_weather_data(); ///< load persistent weather data into *this

public: // helper
  struct wday_hour {
    unsigned char wday = 0, hour = 0; ///< (sun=0...sat=6), (0...23)
  };
  static wday_hour get_wday_hour(); ///< Get current week-day and hour
public: // dev
  void dev_fill_past_wd_randomly();
private:
  Weather_Provider *m_wp = nullptr;
  weather_data m_past_wd[7][24] = {}; ///< past weather data sorted by weekday and hour

public:


  /**
   * \brief  read out member objects in JSON format in chunks
   *
   * This is called from a read function which will call it multiple times until all data was read
   *
   * \param buf       buffer.
   * \param buf_size  should at least 250 bytes
   * \param obj_ct    holds the state (where we were at return). Needs to passed unchanged to next call.
   *                  obj_ct holds the number of objects
   *                  if obj_ct is -1 (EOF), the function does nothing
   * \param start_ct  as long as obj_ct is less than this parameter, do nothing
   *
   * \return          returns the number of bytes written to buf
   *                  returns 0 for EOF or if buffer was not large enough
   * .
   */
  int to_json(char *buf, size_t buf_size, int &obj_ct, int &state, int start_ct = 0);
  static constexpr int PAST_WD_OBJS =  sizeof m_past_wd / sizeof m_past_wd[0][0];
  static constexpr int TOTAL_OBJS = PAST_WD_OBJS;
};
