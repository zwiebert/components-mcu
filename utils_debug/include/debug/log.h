#ifdef MCU_ESP32
#include <esp_log.h>
#define db_loge  ESP_LOGE
#define db_logi  ESP_LOGI
#define db_logw  ESP_LOGW
#define db_logd  ESP_LOGD
#else
#include <stdio.h>
#define db_loge( tag, format, ... ) fprintf(stderr,  "E %s: " format "\n", tag __VA_OPT__(,) __VA_ARGS__)
#define db_logi( tag, format, ... ) fprintf(stderr,  "I %s: " format "\n", tag __VA_OPT__(,) __VA_ARGS__)
#define db_logw( tag, format, ... ) fprintf(stderr,  "W %s: " format "\n", tag __VA_OPT__(,) __VA_ARGS__)
#endif
