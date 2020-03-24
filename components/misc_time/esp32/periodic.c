#include "misc/time/periodic.h"
#include "misc/int_types.h"

extern volatile u32 run_time_s10;
#define get_now_time_ts(x) (run_time_s10 + 0)
#include <time.h>
volatile time_t run_time_secs;
#define get_now_time(x) (run_time_secs + 0)

bool periodic_ts(unsigned interval_ts, unsigned *state) {
  unsigned now = get_now_time_ts();

  if (*state < now) {
    *state = now + interval_ts;
    return true;
  }
  return false;
}

bool periodic(unsigned interval, unsigned *state) {
  unsigned now = get_now_time();

  if (*state < now) {
    *state = now + interval;
    return true;
  }
  return false;
}
