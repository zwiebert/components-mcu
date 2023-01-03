/**
 * \file  main_loop/main_queue.h
 * \brief Execute functions from main task
 */

#pragma once

#include <stdbool.h>

typedef void (*voidFunT)();

/**
 * \brief Setup component
 * \param queue_length  maximal number of items in queue
 * \param event_group   optional FreeRTOS EventGroup to signal messages pushed to queue
 * \param event_bit optional FreeRTOS EventGroup-EventBit used to signal messages pushed to queue
 * 
 */
bool mainLoop_setup(unsigned queue_length, void *event_group = 0, unsigned event_bit = 0);

/**
 * \brief            Process some  or all messages in queue
 * \param max_count  number of messages to process (0 for all)
 * \time_out_ms      Duration to block if called on an empty queue
 * \return           number of messages processed
 */
unsigned mainLoop_processMessages(unsigned max_count = 0, unsigned time_out_ms = 0);

/**
 * \brief            Call a function from main task. Should normally be called from other tasks.
 * \return           success
 */
bool mainLoop_callFun(voidFunT fun);

/**
 * \brief            Call a function from main task. Should be called from interrupt.
 * \return           success
 */
bool mainLoop_callFun_fromISR(voidFunT fun);

/**
 * \brief           Call a function from main task with delay or periodic. Should normally be called from other tasks.
 * \param delay_ms  Delay and/or period in ms
 * \param periodic  if true, call the function periodic. the first call is delayed
 * \return          handle of the delay timer (needed to stop a periodic timer). NULL means failure.
 */
void *mainLoop_callFunByTimer(voidFunT fun, unsigned delay_ms, bool periodic = false);
/**
 * \brief           Stop a periodic function call
 * \param tmr       Handle to timer, as returned by mainLoop_callFun
 * \return          success
 */
bool mainLoop_stopFun(void *tmr, bool delete_timer = true);

/**
 * \brief           Restart MCU from main task.
 * \param delay_ms delay before restart occurs
 * 
 */
void mainLoop_mcuRestart(unsigned delay_ms);
