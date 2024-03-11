#pragma once

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
    float all = 0;   // cloud coverage in percent
  } clouds;

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
  float get_cloud_coverage_percent() const {
    return clouds.all;
  }
};
