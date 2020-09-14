#include "app/config/proj_app_cfg.h"
#include "misc/time/periodic.h"
#include "misc/int_types.h"
#include "misc/time/run_time.h"


bool periodic_ts(unsigned interval_ts, unsigned *state) {
  unsigned now = run_time_ts();
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
  unsigned now = run_time_s();
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
