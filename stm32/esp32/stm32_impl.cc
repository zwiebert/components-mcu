#include "stm32_impl.hh"
#include "driver/gpio.h"
#include <stm32/stm32_bootloader.hh>

#define STM32_SET_BOOT_PIN(level) gpio_set_level((gpio_num_t)stm32_cfg->boot_gpio, (!level != !stm32_cfg->boot_gpio_is_inverse));


/// XXX: move the following to its own source file

static Stm32_Uart_ESP32 stm32_uart_esp32_;
Stm32_Uart_ESP32 *stm32_uart_esp32 = &stm32_uart_esp32_;

#ifdef CONFIG_STM32_MAKE_API_THREAD_SAFE
static Stm32_Uart_Ts stm32_uart_ts_(stm32_uart_esp32_);
Stm32_Uart_if *stm32_uart_ns = &stm32_uart_esp32_;
Stm32_Uart_if *stm32_uart = &stm32_uart_ts_;
Stm32_Bootloader stm32_bootloader(stm32_uart_ts_);
#else
Stm32_Bootloader stm32_bootloader(stm32_uart_esp32_); // XXX: should not be needed (remove later)
Stm32_Uart_if *stm32_uart_ns = &stm32_uart_esp32_;
Stm32_Uart_if *stm32_uart = stm32_uart_ns;
#endif

extern "C" void stm32_setup(const struct cfg_stm32 *stm32_cfg) {
  stm32_uart_esp32->stm32_setup(stm32_cfg);
}

void stm32_wake_up_queue() {
  stm32_uart_esp32->stm32_wake_up_queue();
}
