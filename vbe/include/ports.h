#ifndef _PORTS_H
#define _PORTS_H

#include <types.h>

static inline void outb(unsigned short port, unsigned char value);
static inline void outb(unsigned short port, unsigned char value)
{
    asm volatile ("out %1, %0" : : "dN" (port), "a" (value));
}

static inline void outw(unsigned short port, unsigned short value);
static inline void outw(unsigned short port, unsigned short value)
{
    asm volatile ("out %1, %0" : : "dN" (port), "a" (value));
}

static inline void outd(unsigned short port, unsigned int value);
static inline void outd(unsigned short port, unsigned int value)
{
    asm volatile ("out %1, %0" : : "dN" (port), "a" (value));
}

static inline unsigned char inb(unsigned short port);
static inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    asm volatile ("in %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

static inline unsigned short inw(unsigned short port);
static inline unsigned short inw(unsigned short port)
{
    unsigned short ret;
    asm volatile ("in %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

static inline unsigned int ind(unsigned short port);
static inline unsigned int ind(unsigned short port)
{
    unsigned int ret;
    asm volatile ("in %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

#endif
