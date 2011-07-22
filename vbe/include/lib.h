#ifndef _LIB_H
#define _LIB_H

#include <types.h>

void memset(void *dest, int val, size_t count);
size_t strlen(const char *str);
char* strcpy(char *dest, const char *src);
void* memcpy(void *dest, const void* src, size_t count);

static inline uint32_t abs(int32_t i) { if(i < 0) return -i; else return i; }

#endif
