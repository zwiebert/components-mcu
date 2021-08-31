/**
 * \file  main_loop/main_queue.h
 * \brief Execute functions from main task
 */

#pragma once

#include <stdbool.h>

typedef void (*voidFunT)();


bool mainLoop_setup(unsigned queue_length, void *event_group = 0, unsigned event_bit = 0);

unsigned mainLoop_processMessages(unsigned max_count = 0, unsigned time_out_ms = 0);

bool mainLoop_callFun(voidFunT fun);
bool mainLoop_callFun_fromISR(voidFunT fun);

void  mainLoop_mcuRestart(unsigned delay_ms);
