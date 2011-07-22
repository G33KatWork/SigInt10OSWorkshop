#ifndef VGA_MODE_3_H_
#define VGA_MODE_3_H_

#include <print.h>

extern print_handler print_vgamode3_handler;

void vgamode3_init(void);
void vgamode3_clear(void);
void vgamode3_putChar(char c);

#endif
