/*
 * txtio_setup.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "txtio/inout.h"
#include "txtio_mutex.h"
#include <esp_system.h>
#include <esp32/rom/ets_sys.h>
#include "esp_log.h"

static int es_io_putc(char c) {
  putchar(c);
  return 1;
}

static int  es_io_getc(void) {
 return getchar();
}

void txtio_mcu_setup() {
  txtio_mutexSetup();
  io_putc_fun = es_io_putc;
  io_getc_fun = es_io_getc;
  con_printf_fun = ets_printf;

  esp_log_level_set("*", txtio_config->verbose);
#if 0
  if (TXTIO_IS_VERBOSE(6)) {
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
  }
#endif
}
