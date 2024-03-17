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

float Weather_Irrigation::get_simple_irrigation_factor(int hours) const {
   if (hours > 24 * 7)
     hours = 24 *7;

  const unsigned from_hour = 9, to_hour = 21;

  const auto dh = get_wday_hour();

  int n = 0;
  float sum = 0.0;
  for (int wday = dh.wday;; wday = (wday + 6) % 7)
    for (int hour = dh.hour;; hour = (hour + 23) % 24) {
      if (hours-- <= 0)
        return n ? sum / n : 0.0;
      if (from_hour <= hour && hour <= to_hour) {
        if (auto &wd = get_past_weather_data(wday, hour); wd) {
          sum += wd2rvpt(wd);
          ++n;
        }
      }
    }

  return 0.0;
}

#if 0


Faktor: 1.0 = normal  für T20, W0, Humi=50, Clouds=50
T+ = F-
T- = F+
W+ = F-
H+ = F+
H- = F-
C+ = F+
C- = F-

Formel:

float d_temp = 0.1, d_wind = 0.1, d_humi = 0.1, d_clouds = 0.1;
f -= (temp - 20)/d_temp;
f -= (wind)/d_wind;
f += (humi - 50)/d_humi;
f += (clouds - 50)/d_clouds


#endif
