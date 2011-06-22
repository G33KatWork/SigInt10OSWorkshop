#include <interrupts.h>
#include <print.h>
#include <pic.h>
#include <lib.h>

static isrFunction isrs[I86_IDT_MAX_ENTRY_COUNT];

void interrupts_init()
{
    memset(isrs, 0, sizeof(isrFunction) * I86_IDT_MAX_ENTRY_COUNT);
}

void interrupts_faultHandler(registers_t regs)
{
    if(isrs[regs.int_no] != 0)
        isrs[regs.int_no](&regs);
    else
    {
        print_string_static("Unhandled fault: ");
        print_integer(regs.int_no, 10);
        print_string_static(" - Errcode: ");
        print_integer(regs.err_code, 16);
        print_string_static(" - At: ");
        print_integer(regs.eip, 16);
        print_string_static("\n");
    }
}

void interrupts_interruptHandler(registers_t regs)
{
    pic_notify(regs.int_no);
    
    if(isrs[regs.int_no] != 0)
        isrs[regs.int_no](&regs);
    else
    {
        print_string_static("Unhandled interrupt: ");
        print_integer(regs.int_no, 10);
        print_string_static("\n");
    }
}

void interrupts_registerHandler(uint8_t i, isrFunction func)
{
    isrs[i] = func;
}

void interrupts_unregisterHandler(uint8_t i)
{
    isrs[i] = 0;
}
