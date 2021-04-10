/*
 * int_types.h
 *
 *  Created on: 17.03.2020
 *      Author: bertw
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
enum logiclevel { LOW, HIGH };
#else
#include <stdbool.h>
typedef bool logicLevel;
#define LOW false
#define HIGH true
#endif

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;



enum verbosity {
  vrbNone, vrb1, vrb2, vrb3, vrb4, vrb5, vrbDebug
};

