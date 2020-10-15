/**
 * \file   uout/uo_types.h
 * \brief  API parameter and return types
 */
#pragma once
#include "so_target_bits.h"
#include "so_target_desc.hh"
#include "so_out.h"

#include <uout/so_out.h>
#include <uout/status_json.hh>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

/// \brief pinChange args
typedef struct {
  uint8_t gpio_num; ///< GPIO number of the pin
  bool level; ///< Current pin level
} so_arg_pch_t;


