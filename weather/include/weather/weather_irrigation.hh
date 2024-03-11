#pragma once

#include "weather.hh"

float wd2rvpt(const weather_data &wd);
float wdd2rvpt_avg(const weather_data wdd[24], unsigned from_hour = 9, unsigned to_hour = 21);
