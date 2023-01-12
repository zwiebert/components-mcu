#ifdef __cplusplus
  extern "C++" {
#endif
#pragma once


#include "txtio/inout.h"

extern int unit_testing;
#define UNIT_TEST (unit_testing+0)

#include <assert.h>

#if defined MCU_ESP32 && defined DEBUG
#define db_check_heap()   assert(heap_caps_check_integrity_all(true))
#else
#define db_check_heap()
#endif

#ifdef MCU_ESP32
#include <esp_heap_caps.h>
#include <esp32/rom/ets_sys.h>
#define db_puts(s) ets_printf("%s\n",s)
#define db_printf ets_printf
#else
#define db_printf con_printf
#define db_puts(s) db_printf("%s\n",s)
#endif

void abort_precond(const char *msg, int line);
void abort_postcond(const char *msg, int line);

#if defined NO_PRECOND || defined DISTRIBUTION
#define precond(x)
#else
#define precond(x)  assert(x)
#endif

#if defined NO_POSTCOND  || defined DISTRIBUTION
#define postcond(x)
#else
#define postcond(x) assert(x)
#endif

#define dbg_trace()

#ifndef dbg_printf
#define dbg_printf ets_printf
#endif
#ifndef dbg_trace
#define dbg_trace()   dbg_printf("trace: %s()\n", __func__)
#endif

#ifndef DISTRIBUTION
#define dbg_dbd(x) do { if (TXTIO_IS_VERBOSE(vrbDebug)) { x; } } while(0)
#else
#define dbg_dbd(x)
#endif

#define if_verbose_do(x) do { if (TXTIO_IS_VERBOSE(vrbDebug)) { x; } } while(0)


#define dbg_vpf(x) do { if (TXTIO_IS_VERBOSE(vrbDebug)) { x; } } while(0)




#ifdef __cplusplus
  }
#endif
