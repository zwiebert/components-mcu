#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "time.h"
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "cli/cli.h"
#include "cli/mutex.h"
#include "txtio/inout.h"
#include "misc/int_types.h"

static TaskHandle_t xHandle = NULL;
#define STACK_SIZE  3000


static void cli_task(void *pvParameters) {
  for(;;)
    cli_loop();
}


void cli_setup_task(bool enable) {
  static uint8_t ucParameterToPass;

  if (xHandle) {
    vTaskDelete(xHandle);
    xHandle = NULL;
  }

  if (!enable)
    return;

  xTaskCreate(cli_task, "cli_server", STACK_SIZE, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle);
  configASSERT( xHandle );

}

