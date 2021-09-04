/**
 * \file main_loop/esp32/main_queue.cc
 *
 */
#include "app_config/proj_app_cfg.h"

#include "main_loop/main_queue.hh"
#include <debug/dbg.h>

bool mainLoop_setup(unsigned queue_length, void *event_group, unsigned event_bit) {

  return false;
}

bool mainLoop_callFun(voidFunT fun) {
  fun();
  return true;
}

bool mainLoop_callFun_fromISR(voidFunT fun) {
  fun();
  return true;
}

void *mainLoop_callFun(voidFunT fun, unsigned delay_ms, bool periodic) {
  return nullptr;
}
bool mainLoop_stopFun(void *tmr) {
  return false;
}

unsigned mainLoop_processMessages(unsigned max_count, unsigned time_out) {

  return 0;
}

void mainLoop_mcuRestart(unsigned delay_ms) {
}

