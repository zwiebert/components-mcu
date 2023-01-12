/**
 * \file main_loop/esp32/main_queue.cc
 *
 */


#include "main_loop/main_queue.hh"
#include <debug/dbg.h>
#include "utils_misc/int_macros.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>

#include <esp_system.h>

struct __attribute__ ((packed)) mainLoop_msgT_voidFun {
  voidFunT voidFun;
};

static bool mainLoop_pushMessage(const mainLoop_msgT_voidFun *msg);
static bool mainLoop_pushMessage_fromISR(const mainLoop_msgT_voidFun *msg);

static QueueHandle_t mainLoop_msgQueue;
static volatile EventGroupHandle_t mainLoop_eventGroup;
static volatile EventBits_t mainLoop_eventBits;

bool mainLoop_setup(unsigned queue_length, void *event_group, unsigned event_bit) {
  precond(!mainLoop_msgQueue);

  const unsigned size = sizeof(struct mainLoop_msgT_voidFun);

  mainLoop_eventGroup = static_cast<EventGroupHandle_t>(event_group);
  mainLoop_eventBits = BIT(event_bit);

  if ((mainLoop_msgQueue = xQueueCreate(queue_length, size))) {
    return true;
  }

  return false;
}

static bool mainLoop_pushMessage(const mainLoop_msgT_voidFun *msg) {
  precond(mainLoop_msgQueue);
  precond(msg->voidFun);

  if (xQueueSend(mainLoop_msgQueue, msg, ( TickType_t ) 10) != pdTRUE)
    return false;

  if (mainLoop_eventGroup) {
    if (!xEventGroupSetBits(mainLoop_eventGroup, mainLoop_eventBits))
      return false;
  }

  return true;
}

static bool IRAM_ATTR mainLoop_pushMessage_fromISR(const mainLoop_msgT_voidFun *msg) {
  precond(mainLoop_msgQueue);
  precond(msg->voidFun);

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  BaseType_t xHigherPriorityTaskWoken2 = pdFALSE;
  if (xQueueSendToFrontFromISR(mainLoop_msgQueue, msg, &xHigherPriorityTaskWoken ) != pdTRUE)
    return false;

  if (mainLoop_eventGroup) {
    if (!xEventGroupSetBitsFromISR(mainLoop_eventGroup, mainLoop_eventBits, &xHigherPriorityTaskWoken2))
      return false;
  }

  if (xHigherPriorityTaskWoken || xHigherPriorityTaskWoken2) {
    portYIELD_FROM_ISR();
  }

  return true;
}

bool mainLoop_callFun(voidFunT fun) {
  mainLoop_msgT_voidFun msg { fun };
  return mainLoop_pushMessage(&msg);
}

void *mainLoop_callFunByTimer(voidFunT fun, unsigned delay_ms, bool periodic) {
  TimerHandle_t tmr = xTimerCreate("TxWorker", pdMS_TO_TICKS(delay_ms), (periodic ? pdTRUE : pdFALSE), (void*) fun, [](TimerHandle_t xTimer) {
    voidFunT f = (voidFunT) pvTimerGetTimerID(xTimer);
    mainLoop_callFun(f);
  });
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    return nullptr;
  }

  return (void *)tmr;
}


bool mainLoop_stopFun(void *tmr, bool delete_timer) {
  if (!tmr)
    return false;

  const bool hasStopped = (pdPASS == xTimerStop(static_cast<TimerHandle_t>(tmr), 10));
  if (!delete_timer || !hasStopped)
    return hasStopped;

  return pdPASS == xTimerDelete(static_cast<TimerHandle_t>(tmr), 10);
}

bool IRAM_ATTR mainLoop_callFun_fromISR(voidFunT fun) {
  mainLoop_msgT_voidFun msg { fun };
  return mainLoop_pushMessage_fromISR(&msg);
}

unsigned mainLoop_processMessages(unsigned max_count, unsigned time_out_ms) {
  precond(mainLoop_msgQueue);

  unsigned result = 0;

  mainLoop_msgT_voidFun msg;

  for (int i = 0; max_count == 0 || i < max_count; ++i) {
    if (!xQueueReceive(mainLoop_msgQueue, &msg, (TickType_t) pdMS_TO_TICKS(time_out_ms))) {
      break;
    }
    ++result;

    assert(msg.voidFun);
    (*msg.voidFun)();

  }

  return result;
}

void mainLoop_mcuRestart(unsigned delay_ms) {
  printf("mcu_restart()\n");
  vTaskDelay(pdMS_TO_TICKS(delay_ms));
  esp_restart();
  for (;;) {
  }
}

