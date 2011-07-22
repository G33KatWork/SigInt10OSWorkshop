#ifndef _BYTESWAP_H_
#define _BYTESWAP_H_

#include <types.h>

static inline uint16_t bswap_16(uint16_t x) {
  return (x>>8) | (x<<8);
}

static inline uint32_t bswap_32(uint32_t x) {
  return (bswap_16(x&0xffff)<<16) | (bswap_16(x>>16));
}

#endif
