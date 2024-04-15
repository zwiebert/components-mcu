#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <cstdlib>

#ifdef CONFIG_WEATHER_TEST_DEBUG
#define DEBUG
#define D(x) x
#include <iostream>
#include <iomanip>
using namespace std;
#else
#define D(x)
#endif
#define logtag "weather.test.irrigation"

#include "weather/weather_irrigation.hh"
weather_data wda[24];

void init_wda() {
  for(auto &wd : wda) {
    wd.main.temp = 273 + 20;
    wd.main.humidity = 50;
    wd.clouds.all = 50;
    wd.wind.speed = 5;
  }
}

template<typename T>
T rando(T low_limit, T high_limit) {
  return low_limit + static_cast <T> (rand()) / ( static_cast <T> (RAND_MAX/(high_limit - low_limit)));
}

void init_wd_rand(weather_data &wd) {
    wd.main.temp = 273.15 + rando(5.0, 37.0);
    wd.main.humidity = rando(0, 100);
    wd.clouds.all = rando(0, 100);
    wd.wind.speed = rando(0.0, 85.0);
}
void init_wda_rand() {
  for(auto &wd : wda) {
     init_wd_rand(wd);
  }
}


static void test_points() {
  init_wda();
  auto pts_avg = wdd2rvpt_avg(wda);
  auto pts_sgl = wd2rvpt(wda[0]);
  D(cout << "pts_sgl: " << pts_sgl << "\n");
  D(cout << "pts_avg: " << pts_avg << "\n");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(pts_avg, pts_sgl, "All elements should be equal");
}


static void test_points2() {
  init_wda_rand();
  auto pts_avg = wdd2rvpt_avg(wda);
  auto pts_sgl = wd2rvpt(wda[0]);
  D(cout << "pts_sgl: " << pts_sgl << "\n");
  D(cout << "pts_avg: " << pts_avg << "\n");
  TEST_ASSERT_NOT_EQUAL_FLOAT_MESSAGE(pts_avg, pts_sgl, "Elements should be different");
}

static void test_weather_irrigation() {
  Weather_Irrigation wi;
  wi.dev_fill_past_wd_randomly();
  auto pts_avg = wi.get_simple_irrigation_factor(24*9);
  D(cout << "pts_avg: " << pts_avg << "\n");
}



struct rv_adapter {
  const char *name = "lawn1";
   float d_temp = 0.06, d_wind = 0.03, d_humi = 0.005, d_clouds = 0.01;
};



static void get_factor(const weather_data &wd, const rv_adapter &rva) {
  float f_temp = - ((wd.get_temp_celsius() - 20) * rva.d_temp);
  float f_wind = - ((wd.get_wind_speed_kph()) * rva.d_wind);
  float f_humi = + ((wd.get_relative_humidity_percent() - 50) * rva.d_humi);
  float f_clouds = + ((wd.get_cloud_coverage_percent() - 50) * rva.d_clouds);

  float f = 1.0 + f_temp + f_wind + f_humi + f_clouds;

#ifdef DEBUG
  float upper_limit = 3.0;
    if (f_temp > upper_limit)
      cout << "f_temp: " << f_temp << "\n";
    if (f_wind > upper_limit)
      cout << "f_wind: " << f_wind << "\n";
    if (f_humi > upper_limit)
      cout << "f_humi: " << f_humi << "\n";
    if (f_clouds > upper_limit)
      cout << "f_clouds: " << f_clouds << "\n";
#endif


  D(cout << std::fixed << std::setprecision(2)
      << "temp: " << std::setw(5) << wd.get_temp_celsius()
      << ", wind: " << std::setw(5) << wd.get_wind_speed_kph()
      << ", humi: " << std::setw(3) << wd.get_relative_humidity_percent()
      << ", clouds: " << std::setw(3) << wd.get_cloud_coverage_percent()
      << ", f: " << std::setw(12) << f
      << ", 7 => "<< std::setw(12)  << 7 * f
      << ", 3 => " << std::setw(12) << 3 * f
      << "\n");
}

void init_wd_rand_avg(weather_data &wd) {
    wd.main.temp = 273.15 + rando(5.0, 28.0);
    wd.main.humidity = rando(10, 90);
    wd.clouds.all = rando(0, 100);
    wd.wind.speed = rando(0.0, 30.0);
}

static void test_adaption() {
  weather_data wda[24*4];

  for(auto &wd : wda) {
     init_wd_rand_avg(wd);
  }

  rv_adapter a_lawn = { .d_temp = 0.06, .d_wind = 0.001, .d_humi = 0.0000000001, .d_clouds = 0.0000000001};

  for(auto wd: wda) {
    get_factor(wd, a_lawn);
  }



}


TEST_CASE("test irrigation", "[weather]")
{
  test_adaption();
#if 0
  test_weather_irrigation();
  test_points2();
   test_points();
#endif
}
