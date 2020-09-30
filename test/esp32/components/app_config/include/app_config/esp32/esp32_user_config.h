#pragma once

#define MCU_TYPE "esp32"

//extern const char *const build_date;
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

//#undef FER_TRANSMITTER
//#undef FER_RECEIVER
//#undef USE_NTP
//#undef USE_WLAN
//#undef USE_MQTT
#undef USE_TCPS

#define NO_SPIFFS

#undef CONFIG_BLOB
//#undef ACCESS_GPIO
#define CONFIG_GPIO_SIZE 40
//#undef USE_POSIX_TIME
#undef MDR_TIME
//#undef USE_PAIRINGS

#ifndef FLAVOR_LAN
#undef USE_LAN
#endif

#include "esp_attr.h"


