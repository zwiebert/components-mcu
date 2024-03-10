/*
 weather.c - Weather data retrieval from api.openweathermap.org

 This file is part of the ESP32 Everest Run project
 https://github.com/krzychb/esp32-everest-run

 Copyright (c) 2016 Krzysztof Budzynski <krzychb@gazeta.pl>
 This work is licensed under the Apache License, Version 2.0, January 2004
 See the file LICENSE for details.
 */

#include "weather/weather.hh"

#include "net/http_client.h"

#include <stdio.h>
#include <string.h>

#include "parse_json.hh"

bool weather_fetch_weather_data(weather_data &data, const char *url) {
  const size_t buf_size = 750; // current content_length is 471
  char buf[buf_size];


  if (httpClient_getToBuffer(url, buf, buf_size))
    if (weather_process_json(buf, data)) {
      return true;
    }

  return false;
}
