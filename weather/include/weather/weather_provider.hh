#pragma once

#include "weather_data.hh"

class Weather_Provider {
public:
  /// Fetch weather data from provider
  virtual bool fetch_weather_data(weather_data &data) const = 0;

public:
  virtual ~Weather_Provider() {
  }
};

