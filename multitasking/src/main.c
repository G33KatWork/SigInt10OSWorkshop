#include <gdt.h>
#include <idt.h>
#include <registers.h>
#include <print.h>
#include <global.h>
#include <pic.h>
#include <interrupts.h>
#include <pit.h>
#include <thread.h>

void cpu_init(void);
int kmain(void* mbd, unsigned int magic);
void timerHandler(registers_t* regs);
void thread(uint32_t argument);

void timerHandler(registers_t* regs)
{
	thread_schedule(regs);
}

void thread(uint32_t argument)
{
	for(;;)
	{
		print_string_static("O HAI!\n");
		for(uint32_t i = 0; i < 20000000; i++);
	}
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
    cpu_init();					//set up GDT
    idt_install();				//set up IDT
	interrupts_init();			//set up callback table
	pic_init();					//set up PIC
	interrupts_enable();

    print_clear();
    print_string_static("Hello... I'm a C Kernel!1!!\nAnd I have a PIC and PIT intialized.\n");
	
	interrupts_registerHandler(IRQ_TIMER, timerHandler);
	pit_init(50);
	
	thread_init(0x1900000);
	thread_create(0x2000000, 0x3000000, 0x1000, (uint32_t)thread, 0x61);
	
	pic_unmask_irq(IRQ_TIMER);

	for(;;)
	{
		print_string_static("KTHX!\n");
		for(uint32_t i = 0; i < 20000000; i++);
	}
    return 0;
}
