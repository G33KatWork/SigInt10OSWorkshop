#ifndef _V86_H_
#define _V86_H_

#include <types.h>

//only lowermost MB usable
//we have no paging and an identity GDT, so this is essentially physical memory
//0x00007E00 to 0x0007FFFF are guaranteed to be free
#define REALMODECODE      0x00007E00
#define REALMODESTACK_END 0x00070000

#define REALMODE_PTR(segment, offset)   ((void*)((((uint16_t)segment) << 4) + ((uint16_t)offset)))

struct v86_biosArguments
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    
    uint16_t ds;
    uint16_t es;
};

void v86_init(void);
void v86_bioscall(uint8_t intNum, struct v86_biosArguments arguments, struct v86_biosArguments* results);

#endif
