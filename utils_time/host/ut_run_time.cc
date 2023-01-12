#include <utils_time/run_time.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#undef run_time_ts
#undef run_time_s

constexpr uint64_t NSEC_IN_SEC = 1'000'000'000UL;
constexpr uint64_t MSEC_IN_SEC = 1000;
constexpr uint64_t TSEC_IN_SEC = 10;
constexpr uint64_t NSEC_IN_MSEC = 1'000'000UL;
constexpr uint64_t MSEC_IN_TSEC = 100;

#pragma GCC push_options
//#pragma GCC optimize ("O0")

static time_t start_ts;  ///< initial value at program start (may be not zero)
static time_t slept_ts;  ///< accumulate pseudo-sleep() times

static uint64_t get_ts(void) {
  struct timespec spec;
  clock_gettime(CLOCK_MONOTONIC, &spec);

  uint64_t ms = spec.tv_nsec / NSEC_IN_MSEC;
  ms += spec.tv_sec * MSEC_IN_SEC;

  return ms / MSEC_IN_TSEC;
}

struct Init {
  Init() {
    start_ts = get_ts();
  }

};

static Init init;

uint32_t run_time_ts() {
  time_t ts = get_ts();
  ts -= start_ts;
  ts += slept_ts;
  return ts;
}

uint32_t run_time_s() {
  return run_time_ts() / TSEC_IN_SEC;
}

void ut_sleep_ts(unsigned tsecs) {
  slept_ts += tsecs;
}

void ut_sleep(unsigned secs) {
  slept_ts += TSEC_IN_SEC * secs;
}

#pragma GCC pop_options
