#include <utils_time/run_time.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#pragma GCC push_options
//#pragma GCC optimize ("O0")

static time_t start_ts;



static long get_ms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_MONOTONIC, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    return ms + s * 1000;
}

static long get_ts (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_MONOTONIC, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e8); // Convert nanoseconds to s/10
    return ms + s * 10;
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
  return ts;
}

uint32_t run_time_s() {
  time_t ts = get_ts();
  ts -= start_ts;
  return ts / 10;
}

#pragma GCC pop_options
