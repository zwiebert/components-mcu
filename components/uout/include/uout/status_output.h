#ifdef __cplusplus
  extern "C" {
#endif

#pragma once

#include <stdint.h>

// to select target for output messages
typedef enum {
  SO_TGT_NONE = 0,
  SO_TGT_CLI_USB = (1<<0),
  SO_TGT_CLI_TCP = (1<<1),
  SO_TGT_CLI = (SO_TGT_CLI_USB|SO_TGT_CLI_TCP),
  SO_TGT_HTTP = (1<<2),
  SO_TGT_MQTT = (1<<3),
  SO_TGT_FLAG_JSON = (1<<4),
  SO_TGT_FLAG_TXT = (1<<5),
  SO_TGT_WS = (1<<6),
  SO_TGT_STM32 = (1<<7),
  SO_TGT_ANY = (SO_TGT_CLI|SO_TGT_HTTP|SO_TGT_MQTT|SO_TGT_WS)
} so_target_bits;

extern uint8_t so_target;

#define so_tgt_set(bitmask) (so_target = (bitmask))
#define so_tgt_default() (so_tgt_set(SO_TGT_ANY))
#define so_tgt_test(bitmask) (so_target & (bitmask))

#define cli_isInteractive() (so_tgt_test(SO_TGT_CLI) &&  so_tgt_test(SO_TGT_FLAG_TXT))

#ifdef USE_WS
void ws_send_json(const char *json); //XXX:
#endif
#ifdef __cplusplus
  }
#endif
