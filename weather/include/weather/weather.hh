#pragma once
#include "weather_provider.hh"
#include "weather_data.hh"

class Weather {
public:
  bool set_weather_provider(Weather_Provider *wp); ///< provider to fetch data from
public:
  /**
   * \brief            Get past weather data for given point in time
   * \param week_day   day of the week (sun=0...sat=6)
   * \param hour       hour of day (0...23)
   * \return           Filled in weather_data object (success) -or- empty object (no data was available)
   */
  const weather_data& get_past_weather_data(unsigned week_day, unsigned hour);

  /***
   * \brief       Analyze weather data (temperature, wind, clouds) of last 7 days
   * \return      calculated factor which can be used to shorten or lengthen garden irrigation intervals
   */
  float get_simple_irrigation_factor() const; //XXX: should go into separate class:


public:
  bool fetch_and_store_weather_data(); ///< get weather data and store it persistently
  bool load_past_weather_data(); ///< load persistent weather data into *this

public: // helper
  struct wday_hour {
    unsigned char wday = 0, hour = 0; ///< (sun=0...sat=6), (0...23)
  };
  static wday_hour get_wday_hour(); ///< Get current week-day and hour
private:
  Weather_Provider *m_wp = nullptr;
  weather_data m_past_wd[7][24] = {}; ///< past weather data sorted by weekday and hour
};
