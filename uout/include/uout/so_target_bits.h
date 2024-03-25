/**
 * \file    uout/so_target_bits.h
 * \brief   flags to select output formats and targets
 */

#pragma once

/**
 * \brief  select target and specify data-format for output messages
 */
typedef enum {
  SO_TGT_NONE = 0, ///< write to no target
  SO_TGT_CLI_USB = (1<<0), ///< write to local console
  SO_TGT_CLI_TCP = (1<<1), ///< write to remote console
  SO_TGT_CLI = (SO_TGT_CLI_USB|SO_TGT_CLI_TCP),
  SO_TGT_HTTP = (1<<2), ///< write to HTTP (XXX:what use???)
  SO_TGT_MQTT = (1<<3), ///< write to MQTT server
  SO_TGT_FLAG_JSON = (1<<4), ///< format output data to be JSON
  SO_TGT_FLAG_TXT = (1<<5), ///< format output data to be plain text
  SO_TGT_WS = (1<<6), ///< write to web socket
  SO_TGT_STM32 = (1<<7), ///< write to STM32 via UART
  SO_TGT_ANY = (SO_TGT_CLI|SO_TGT_HTTP|SO_TGT_MQTT|SO_TGT_WS)  ///< write to all targets
} so_target_bits;

constexpr so_target_bits operator|(so_target_bits lhs, so_target_bits rhs) {
  return static_cast<so_target_bits>((unsigned)lhs | (unsigned)rhs);
}

#define so_tgt_test(bitmask) (td.tgt() & (bitmask))
#define cli_isInteractive() (so_tgt_test(SO_TGT_CLI) &&  so_tgt_test(SO_TGT_FLAG_TXT))
