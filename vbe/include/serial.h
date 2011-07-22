#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <types.h>
#include <print.h>

#define SERIAL_COM1  0x3f8
#define SERIAL_COM2  0x2f8

extern print_handler print_serial_handler;

void serial_init(void);
void serial_clear(void);
void serial_putChar(char c);

#endif
