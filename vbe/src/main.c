#include <gdt.h>
#include <idt.h>
#include <registers.h>
#include <print.h>
#include <global.h>
#include <pic.h>
#include <interrupts.h>
#include <pit.h>
#include <v86.h>
#include <lib.h>

void cpu_init(void);
int kmain(void* mbd, unsigned int magic);
void timerHandler(registers_t* regs);

void timerHandler(registers_t* UNUSED(regs))
{
	print_string_static("TickTack\n");
}

void cpu_init()
{
    gdt_set_descriptor(0, 0, 0, 0, 0);
	gdt_set_descriptor (1,0,0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_PRESENT,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);
	gdt_set_descriptor (2,0,0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_PRESENT,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);
	
    gdt_install();
}

struct vbeControllerInfo {
   char signature[4];             // == "VESA"
   short version;                 // == 0x0300 for VBE 3.0
   short oemString[2];            // isa vbeFarPtr
   unsigned char capabilities[4];
   short videomodes[2];           // isa vbeFarPtr
   short totalMemory;             // as # of 64KB blocks
};

int kmain(void* UNUSED(mbd), unsigned int UNUSED(magic))
{
    print_clear();
    print_string_static("v86 demo\n");
    
    cpu_init();					//set up GDT
    idt_install();				//set up IDT
	interrupts_init();			//set up callback table
	pic_init();					//set up PIC
    
    v86_init();
    print_string_static("v86 initialized\n");
    
    struct v86_biosArguments results = {.eax = 0x4f00, .es = 0x2000, .edi = 0};
    v86_bioscall(0x10, results, &results);
    
    if(results.eax == 0x4f)
    {
        print_string_static("Query for VBE controller info was successful:\n");
        
        struct vbeControllerInfo* info = (struct vbeControllerInfo*)0x20000;
        print_string_static("VBE Signature:");
        print_string(info->signature);
        print_string_static("\nVBE Version:");
        print_integer(info->version, 16);
    }
    else
        print_string_static("Query for VBE controller info was not successful - wrong return value in AX :'(\n");
    
	/*interrupts_enable();
	interrupts_registerHandler(IRQ_TIMER, timerHandler);
	pit_init(50);
	pic_unmask_irq(IRQ_TIMER);*/
    
	for(;;);
    return 0;
}
