#pragma once
#include "so_target_bits.h"
#include "so_target_desc.hh"
#include "so_out.h"

#include <uout/so_out.h>
#include <uout/status_json.hh>
#include <stdint.h>
#include <unistd.h>
#include <string.h>




//extern uint8_t so_target;

#define so_tgt_set(bitmask) (so_target = (bitmask))
#define so_tgt_default() (so_tgt_set(SO_TGT_ANY))
#define so_tgt_test(bitmask) (td.tgt() & (bitmask))

#define cli_isInteractive() (so_tgt_test(SO_TGT_CLI) &&  so_tgt_test(SO_TGT_FLAG_TXT))

#ifdef USE_WS
void ws_send_json(const char *json, size_t len = 0); //XXX:
#endif

typedef struct {
  uint8_t gpio_num;
  bool level;
} so_arg_pch_t;


