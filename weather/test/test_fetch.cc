#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <iostream>
using namespace std;

#include "weather/weather.hh"

void test_fetch() {
  weather_data wd;
  // test needs  URL with vaild APPID for api.openweathermap.org
#ifdef CONFIG_OPENWEATHERMAP_URL_STRING
  TEST_ASSERT_TRUE_MESSAGE(weather_fetch_weather_data(wd), "Fetchin failed");
  TEST_ASSERT_INT_WITHIN_MESSAGE(30+40, -30, wd.get_temp_celsius(), "Impossible temperature (C)");
  TEST_ASSERT_UINT_WITHIN_MESSAGE(100, 0,wd.get_relative_humidity_percent(), "Humidity out of range");
  cout << "temp (C): " << wd.get_temp_celsius() << "\n";
  cout << "humi (%): " << wd.get_relative_humidity_percent() << "\n";
#endif
}

TEST_CASE("test json", "[weather]")
{
  test_fetch();
}
