#include <weather/weather_irrigation.hh>

#include <cassert>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <iostream>


// old simple method used in home-server script
// the average points are calculated by the rvpt of a single day, and
// only the hours from 9 to 21 are used
// the automated irrigation is supposed to happen in the hours from 22 to 8
// (the last one should happen before sun goes up and the other ones are pushed back
// because as far as the water limits require it.
float wd2rvpt(const weather_data &wd) {
  if (!wd)
    return 0.0;
  return 1.0 * wd.get_temp_celsius() * (wd.get_wind_speed_kph() / 30.0 + 1)
      / (wd.get_cloud_coverage_percent() / 100.0 + wd.get_relative_humidity_percent() / 100.0 + 1);
}

float wdd2rvpt_avg(const weather_data wdd[24], unsigned from_hour, unsigned to_hour) {
  int n = 0;
  float sum = 0.0;
  for (int i = from_hour; i <= to_hour; ++i) {
    float pt = wd2rvpt(wdd[i]);
    if (pt > 0.001) {
      ++n;
      sum += pt;
    }
  }

  return n ? sum / n : 0.0;
}
