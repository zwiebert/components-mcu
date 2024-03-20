#pragma once

#include "weather.hh"

float wd2rvpt(const weather_data &wd);
float wdd2rvpt_avg(const weather_data wdd[24], unsigned from_hour = 9, unsigned to_hour = 21);

class Weather_Adapter_Neutral  {
public:
  virtual ~Weather_Adapter_Neutral() = default;
public:
  virtual float get_factor(const weather_data &wd) const { return 1.0; }
};
class Weather_Irrigation : public Weather {
public:
  /***
   * \brief        Analyze weather data (temperature, wind, clouds) of a given week day between 9:00 and 21:00
   * \param  hours hours in the past we use (at the moment not more than 24*7 hours, which are 7 days are supported)
   * \return       calculated factor which can be used to shorten or lengthen garden irrigation intervals
   */
  float get_simple_irrigation_factor(int hours = 24,
      const Weather_Adapter_Neutral &adapter = Weather_Adapter_Neutral()) const;
};
