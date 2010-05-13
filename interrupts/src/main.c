#include <gdt.h>
#include <idt.h>
#include <registers.h>
#include <print.h>
#include <global.h>

void cpu_init(void);
int kmain(void* mbd, unsigned int magic);

void fault_handler(registers_t regs);
void interrupt_handler(registers_t regs);

void fault_handler(registers_t regs)
{
    print_string_static("Fault occured: ");
    print_integer(regs.int_no);
    print_string_static(" - ");
    print_string_static("EIP: ");
    print_integer(regs.eip);
    print_char('\n');
    while(1);
}

void interrupt_handler(registers_t regs)
{
    print_string_static("Interrupt occured: ");
    print_integer(regs.int_no);
    print_char('\n');
}

void cpu_init()
{
    gdt_set_descriptor(0, 0, 0, 0, 0);
	gdt_set_descriptor (1,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_EXEC_CODE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);
	gdt_set_descriptor (2,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);
	
    gdt_install();
}

int kmain(void* UNUSED(mbd), unsigned int UNUSED(magic))
{
    cpu_init();
    idt_install();

    print_clear();
    print_string_static("Hello... I'm a C Kernel!1!!\n");
    
    asm volatile ("sti");       //enable interrupts
    asm volatile ("int $35");   //fire interrupt 35 (decimal)

    int b = 2-1;
    int a = 1-b;
    int c = b/a;
    print_integer(c);

    return 0;
}
