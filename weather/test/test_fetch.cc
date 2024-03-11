#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

#include <iostream>
using namespace std;

#include "../include/weather/weather_provider_owm.hh"

#include "weather/weather.hh"
void test_weather() {
  Weather_Provider_Owm wp;
  Weather w;
  auto tdh = Weather::get_wday_hour();

  auto pwd0 = w.get_past_weather_data(tdh.wday, tdh.hour);
  cout << "temp (C): " << pwd0.get_temp_celsius() << "\n";
  cout << "humi (%): " << pwd0.get_relative_humidity_percent() << "\n";
  TEST_ASSERT_FALSE_MESSAGE(pwd0, "Weather data is still empty");



  TEST_ASSERT_TRUE_MESSAGE(w.set_weather_provider(&wp), "Make w use provider wp");
  TEST_ASSERT_TRUE_MESSAGE(w.fetch_and_store_weather_data(), "Fetch current data from wp into w, and save to storage");


  auto pwd = w.get_past_weather_data(tdh.wday, tdh.hour);
  cout << "temp (C): " << pwd.get_temp_celsius() << "\n";
  cout << "humi (%): " << pwd.get_relative_humidity_percent() << "\n";
  TEST_ASSERT_TRUE_MESSAGE(pwd, "Weather data should not be empty");
  TEST_ASSERT_INT_WITHIN_MESSAGE(30+40, -30, pwd.get_temp_celsius(), "Test for impossible temperature (C)");

  TEST_ASSERT_TRUE_MESSAGE(w.load_past_weather_data(), "Load data from storage into w");
  weather_data pwd_cpy = w.get_past_weather_data(tdh.wday, tdh.hour);
  TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&pwd_cpy, &pwd, sizeof pwd, "Compare current data against immediately restored");

  Weather w2;
  //TEST_ASSERT_TRUE_MESSAGE(w2.set_weather_provider(&wp), "Make w2 use provider wp");
  TEST_ASSERT_TRUE_MESSAGE(w2.load_past_weather_data(), "Load data from storage into w2");


  auto pwd2 = w2.get_past_weather_data(tdh.wday, tdh.hour);
  cout << "temp (C): " << pwd2.get_temp_celsius() << "\n";
  cout << "humi (%): " << pwd2.get_relative_humidity_percent() << "\n";
  TEST_ASSERT_TRUE_MESSAGE(pwd2, "Weather data should not be empty");
  TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&pwd, &pwd2, sizeof pwd, "Compare restored data against saved");


}

void test_fetch() {
  Weather_Provider_Owm w;
  weather_data wd;

  TEST_ASSERT_TRUE_MESSAGE(w.fetch_weather_data(wd), "Fetchin failed");
  TEST_ASSERT_INT_WITHIN_MESSAGE(30+40, -30, wd.get_temp_celsius(), "Impossible temperature (C)");
  TEST_ASSERT_UINT_WITHIN_MESSAGE(100, 0,wd.get_relative_humidity_percent(), "Humidity out of range");
  cout << "temp (C): " << wd.get_temp_celsius() << "\n";
  cout << "humi (%): " << wd.get_relative_humidity_percent() << "\n";
}

TEST_CASE("test json", "[weather]")
{
// test needs  URL with vaild APPID for api.openweathermap.org
#ifdef CONFIG_OPENWEATHERMAP_URL_STRING
  test_fetch();
  test_weather();
#endif
}
