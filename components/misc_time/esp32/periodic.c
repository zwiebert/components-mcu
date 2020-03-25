#include "misc/time/periodic.h"
#include "misc/int_types.h"
#include "esp_timer.h"

bool periodic_ts(unsigned interval_ts, unsigned *state) {
  unsigned now = esp_timer_get_time() / 100000;
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
  unsigned now = esp_timer_get_time() / 1000000;
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
