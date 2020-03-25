#include "misc/time/periodic.h"
#include "misc/int_types.h"

extern volatile u32 run_time_s10; // uptime counter in s/10 provided by app

bool periodic_ts(unsigned interval_ts, unsigned *state) {
  unsigned now = run_time_s10;

  if (*state == 0) {
    *state = now + interval_ts;
    return false;
  }

  if (*state < now) {
    *state = now + interval_ts;
    return true;
  }
  return false;
}

bool periodic(unsigned interval, unsigned *state) {
  unsigned now = run_time_s10 / 10;

  if (*state == 0) {
    *state = now + interval;
    return false;
  }

  if (*state < now) {
    *state = now + interval;
    return true;
  }
  return false;
}
