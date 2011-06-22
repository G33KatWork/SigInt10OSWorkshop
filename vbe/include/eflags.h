#ifndef _EFLAGS_H_
#define _EFLAGS_H_

#define     EFLAGS_PROTECTED    0x3F7FD5    //0b00000000001111110111111111010101
#define     EFLAGS_REAL         0x7FD5

#define     EFLAGS_CF           (1 << 0)    //Carry Flag
#define     EFLAGS_PF           (1 << 2)    //Parity Flag
#define     EFLAGS_AF           (1 << 4)    //Adjust Flag
#define     EFLAGS_ZF           (1 << 6)    //Zero Flag
#define     EFLAGS_SF           (1 << 7)    //Sign Flag
#define     EFLAGS_TF           (1 << 8)    //Trap Flag
#define     EFLAGS_IF           (1 << 9)    //Interrupt Flag
#define     EFLAGS_DF           (1 << 10)   //Direction Flag
#define     EFLAGS_OF           (1 << 11)   //Overflow Flag
#define     EFLAGS_NT           (1 << 14)   //Nested task flag

#define     EFLAGS_IOPL_MASK    (3 << 12)   //IOPL Mask


#define     EFLAGS_RF           (1 << 16)   //Resume flag
#define     EFLAGS_VM           (1 << 17)   //Virtual86
#define     EFLAGS_AC           (1 << 18)   //Alignment check
#define     EFLAGS_VIF          (1 << 19)   //Virtual interrupt flag
#define     EFLAGS_VIP          (1 << 20)   //Virtual interrupt pending
#define     EFLAGS_ID           (1 << 21)   //CPUID instruction present


#endif
