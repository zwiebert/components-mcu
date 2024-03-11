#pragma once

#include "weather_provider.hh"
#include <cstdlib>

class Weather_Provider_Random final: public Weather_Provider {
  template<typename T>
  T rando(T low_limit, T high_limit) {
    return low_limit + static_cast<T>(rand()) / (static_cast<T>(RAND_MAX / (high_limit - low_limit)));
  }
public:
  virtual bool fetch_weather_data(weather_data &data) const { ///< fetch weather data using our URL member
    wd.main.temp = 273.15 + rando(5.0, 37.0);
    wd.main.humidity = rando(0, 100);
    wd.clouds.all = rando(0, 100);
    wd.wind.speed = rando(0, 85);

    return true;
  }
};

