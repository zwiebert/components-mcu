#ifdef MCU_ESP32
#include <esp_log.h>
#define db_loge  ESP_LOGE
#define db_logi  ESP_LOGI
#else
#include <stdio.h>
#define db_loge( tag, format, ... ) fprintf(stderr,  "E %s: " format "\n", tag __VA_OPT__(,) __VA_ARGS__)
#define db_logi( tag, format, ... ) fprintf(stderr,  "I %s: " format "\n", tag __VA_OPT__(,) __VA_ARGS__)
#endif
