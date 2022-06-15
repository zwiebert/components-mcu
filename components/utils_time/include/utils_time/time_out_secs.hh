/**
 * \file     utils_time/time_out_secs.h
 * \brief    class for polling a time_out in seconds
 *
 */
#include "run_time.h"

#pragma once

class Time_Out_Secs {
public:
  bool isTimeoutRunning() const {
    if (!end_time)
      return false;

    if (end_time < run_time_s()) {
      return false;
    }

    return true;
  }
  bool isTimeoutReached() {
    if (!end_time)
      return false;

    if (end_time < run_time_s()) {
      end_time = 0;
      return true;
    }
    return false;
  }

  void clear() {
    end_time = 0;
  }
  void set(unsigned secs) {
    end_time = run_time_s() + secs;
  }
public:
  Time_Out_Secs() {
  }

  Time_Out_Secs(unsigned secs) :
      end_time(run_time_s() + secs) {
  }
private:
  unsigned long end_time = 0;
};
