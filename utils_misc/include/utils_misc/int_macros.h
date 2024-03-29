/**
 * \file utils_misc/int_macros.h
 * \brief Basic helper macros for integers
 */
#include <stdint.h>

#pragma once

#ifndef MIN
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif
#ifndef MAX
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#define IS_IN_RANGE(lowLim, val, highLim) (((lowLim) <= (val)) && ((val) <= (highLim)))

#define GET_BYTE_0(src) (uint8_t)(((src)>>0)&0xff)
#define GET_BYTE_1(src) (uint8_t)(((src)>>8)&0xff)
#define GET_BYTE_2(src) (uint8_t)(((src)>>16)&0xff)
#define GET_BYTE_3(src) (uint8_t)(((src)>>24)&0xff)

#define PUT_LOW_NIBBLE(dst, val) ((dst) = ((dst)&0xf0)|((val)&0x0f))
#define PUT_HIGH_NIBBLE(dst, val) ((dst) = (((val)<<4)&0xf0)|((dst)&0x0f))
#define GET_LOW_NIBBLE(src) ((src)&0x0f)
#define GET_HIGH_NIBBLE(src) (((src)>>4)&0x0f)

#define PUT_LOW_BYTE(dst, val) ((dst) = ((dst)&0xff00)|((val)&0x00ff))
#define PUT_HIGH_BYTE(dst, val) ((dst) = (((val)<<8)&0xff00)|((dst)&0x00ff))
#define GET_LOW_BYTE(src) ((src)&0x00ff)
#define GET_HIGH_BYTE(src) (((src)>>8)&0x00ff)

#define GET_BIT(var,pos) (0 != ((var) & (1<<(pos))))
#define SET_BIT(var,pos) ((var) |= (1 << (pos)))
#define CLR_BIT(var,pos) ((var) &= ~((1) << (pos)))
#define PUT_BIT(var,pos, val) ((val) ? SET_BIT(var,pos) : CLR_BIT(var,pos))

#define BIT(nr)  (1UL << (nr))

#define GET_BIT64(var,pos) (0 != ((var) & (1ULL<<(pos))))
#define SET_BIT64(var,pos) ((var) |= (1ULL << (pos)))
#define CLR_BIT64(var,pos) ((var) &= ~((1ULL) << (pos)))
#define PUT_BIT64(var,pos, val) ((val) ? SET_BIT64(var,pos) : CLR_BIT64(var,pos))
#define BIT64(nr)  (1ULL << (nr))


