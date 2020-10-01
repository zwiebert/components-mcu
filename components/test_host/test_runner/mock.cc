#include <misc/int_types.h>

void  mcu_restart(void) {}
bool mcu_get_buttonPin(void) { return false; }
volatile u32 run_time_s_, run_time_ts_;
void setup_pin(const struct cfg_gpio *c) {}
void mcu_put_txPin(unsigned char) {}
