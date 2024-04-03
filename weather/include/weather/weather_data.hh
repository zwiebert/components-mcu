#pragma once
#include <cstdio>

struct weather_data {
  struct {
    unsigned humidity = 0;  // relative humidity in percent
    float temp = 0;  // air temperature in Kelvin
    unsigned pressure = 0;  // air pressure in mBar (== hPa)
  } main;
  struct {
    float speed = 0;  // wind speed in km/h
    unsigned int deg = 0;  // wind direction in degrees
  } wind;
  struct {
    unsigned all = 0;   // cloud coverage in percent
  } clouds;

public:
  operator bool() const {return main.temp != 0; }
public:
  float get_temp_celsius() const {
    return main.temp - 273.15;
  }
  unsigned get_relative_humidity_percent() const {
    return main.humidity;
  }
  float get_wind_speed_kph() const {
    return wind.speed;
  }
  unsigned get_cloud_coverage_percent() const {
    return clouds.all;
  }
public:
  int to_json(char *dst, size_t dst_size) const {
    auto n = snprintf(dst, dst_size, //
        R"({"main":{"humidity":%u,"temp":%f,"pressure":%u},"wind":{"speed":%f,"deg":%u},"clouds":{"all":%u}})", //
        main.humidity, main.temp, main.pressure, //
        wind.speed, wind.deg, //
        clouds.all);

    return n < dst_size ? n : 0;
  }
};
