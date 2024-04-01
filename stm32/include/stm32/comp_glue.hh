/**
 * \brief  optional header only glue inline functions, to glue components together.
 *         This header is supposed  to be included by an application.
 *
 *  To access this file, the caller need to have required modules: uout and config_kvs
 *  
 *
 */

#pragma once

#if __has_include(<config_kvs/settings.hh>) &&  __has_include(<uout/so_target_desc.hh>)
#include <stm32/stm32.h>
#include <config_kvs/settings.hh>
#include <config_kvs/comp_settings.hh>
#include <app_config/options.hh>

#ifdef CONFIG_STM32_USE_COMPONENT

inline cfg_stm32 *config_read_stm32(cfg_stm32 *cfg) {
  *cfg = cfg_stm32 { .uart_tx_gpio = CONFIG_STM32_UART_TX_PIN, .uart_rx_gpio = CONFIG_STM32_UART_RX_PIN, .boot_gpio_is_inverse =
      !!config_read_item((CB_STM32_INV_BOOTPIN), 0), .boot_gpio =  CONFIG_STM32_BOOT_PIN, .reset_gpio = CONFIG_STM32_RESET_PIN, };
  return cfg;
}

inline void config_setup_stm32() {
  struct cfg_stm32 c;
  config_read_stm32(&c);
  stm32_setup(&c);
}

inline bool config_read_stm32_inv_bootpin() {
  return !!config_read_item((CB_STM32_INV_BOOTPIN), 0);
}

#include <uout/so_target_desc.hh>
inline void soCfg_STM32_BOOTGPIO_INV(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_STM32_INV_BOOTPIN), config_read_stm32_inv_bootpin());
}
#endif

template<typename settings_type>
constexpr void stm32_register_settings(settings_type &settings) {
#ifdef CONFIG_STM32_USE_COMPONENT
    settings.initField(CB_STM32_INV_BOOTPIN, "C_STM_INV_BP", otok::k_stm32_bootgpio_inv, CBT_i8, soCfg_STM32_BOOTGPIO_INV, STF_direct);
#endif
}

#endif
