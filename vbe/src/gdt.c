#include <gdt.h>
#include <lib.h>
#include <lib.h>
#include <print.h>

struct gdt_entry gdt[I86_GDT_ENTRY_COUNT];
struct gdt_ptr gp;

void gdt_set_descriptor(uint16_t i, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    //print_string_static("gdt_set_descriptor()\n");
    if (i > I86_GDT_ENTRY_COUNT)
		return;

    //print_string_static("setting...\n");

	// null out the descriptor
	memset((void*)&gdt[i], 0, sizeof (struct gdt_entry));

	// set limit and base addresses
	gdt[i].base_low		= (uint16_t)(base & 0xffff);
	gdt[i].base_middle	= (uint8_t)((base >> 16) & 0xff);
	gdt[i].base_high	= (uint8_t)((base >> 24) & 0xff);
	gdt[i].limit_low	= (uint16_t)(limit & 0xffff);

	// set flags and grandularity bytes
	gdt[i].access		 = access;
	gdt[i].granularity	 = (uint8_t)((limit >> 16) & 0x0f);
	gdt[i].granularity	|= granularity & 0xf0;
	
	
	/*struct gdt_entry* gdt_ent = &gdt[i];
    uint32_t addr = (uint32_t)gdt_ent;
    uint32_t val = *((uint32_t*)addr);
	
	
    print_integer(val);
    print_string_static("\n");
    
    
    addr += 4;
    val = *((uint32_t*)addr);
    
    print_integer(val);
    print_string_static("\n");*/
}

void gdt_clear_tss_busy_flag(uint16_t i)
{
    gdt[i].access &= 0xFD;
}

void gdt_install(void)
{
    //update pointer
    gp.size = (sizeof(struct gdt_entry) * I86_GDT_ENTRY_COUNT) - 1;
	gp.offset = (uint32_t)&gdt;
	
	//load gdt
    gdt_flush();
}
