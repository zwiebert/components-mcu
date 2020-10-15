// User configuration
#pragma once

#define APP_VERSION "0.5.0"

#define USE_HTTP
#define USE_LAN
#define USE_WLAN
#define USE_WLAN_AP
#define USE_NTP
#define USE_JSON
#define USE_MQTT
#define USE_WS
#define USE_POSIX_TIME
#define POSIX_TIME
#define USE_SERIAL
#define CONFIG_DICT
#define USE_WDG
#define USE_FS
#define USE_HTTP_CLIENT
#define USE_OTA
//#define USE_TCPS
#define USE_EG

#define USE_MUTEX
#define USE_NETWORK


#define STM32_RESET_PIN  (GPIO_NUM_33)
#define STM32_BOOT_PIN (GPIO_NUM_32)

#define STM32_UART_TX_PIN  (GPIO_NUM_4)
#define STM32_UART_RX_PIN  (GPIO_NUM_36)


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "esp32/rom/ets_sys.h"
#include "utils_misc/int_types.h"
#include "txtio/inout.h"

#define MCU_TYPE "esp32"
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)



