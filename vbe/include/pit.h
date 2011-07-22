#ifndef _PIT_H_
#define _PIT_H_

#include <types.h>

void pit_init(uint32_t frequency);
uint16_t pit_getCurrentCount(void);

#endif
