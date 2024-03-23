/*
 weather.c - Weather data retrieval from api.openweathermap.org

 This file is part of the ESP32 Everest Run project
 https://github.com/krzychb/esp32-everest-run

 Copyright (c) 2016 Krzysztof Budzynski <krzychb@gazeta.pl>
 This work is licensed under the Apache License, Version 2.0, January 2004
 See the file LICENSE for details.
 */

#include "weather/weather.hh"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "net/http_client.h"

#include <stdio.h>
#include <string.h>



#if 0

static const char *TAG = "Weather";

static const char *URL = CONFIG_WEATHER_OWM_URL_STRING;

static weather_data_callback data_retreived_cb;
static unsigned long retreival_period;


static void weather_retrieval_task(void *pvParameter) {
  const size_t buf_size = 512;
  char *buf = (char*) malloc(buf_size);

  for (;;) {
    if (httpClient_getToBuffer(URL, buf, buf_size)) {
      weather_data w;

      if (weather_process_json(buf, w)) {
        if (data_retreived_cb) {
          data_retreived_cb(&w);
        }
      }
    }
  }
  vTaskDelay(retreival_period / portTICK_PERIOD_MS);
}

void weather_on_weather_data_retrieval(weather_data_callback cb) {
  data_retreived_cb = cb;
}

void weather_initialise_weather_data_retrieval_task(unsigned long period) {
  retreival_period = period;

  xTaskCreate(&weather_retrieval_task, "weather_retrieval_task", 2 * 2048, NULL, 5, NULL);
  ESP_LOGI(TAG, "weather retrieval task started");
}



#endif
