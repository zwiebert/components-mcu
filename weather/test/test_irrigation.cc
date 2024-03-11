#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <cstdlib>


#define DEBUG

#ifdef DEBUG
#define D(x) x
#include <iostream>
using namespace std;
#else
#define D(x)
#endif

#include "weather/weather_irrigation.hh"
weather_data wda[24];

void init_wda() {
  for(auto &wd : wda) {
    wd.main.temp = 273 + 27;
    wd.main.humidity = 90;
    wd.clouds.all = 20;
    wd.wind.speed = 20;
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
    wd.wind.speed = rando(0, 85);
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

TEST_CASE("test irrigation", "[weather]")
{
  test_points2();
   test_points();
}
