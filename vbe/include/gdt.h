#ifndef _ARCH_GDT_H
#define _ARCH_GDT_H

#include <types.h>

// count of gdt entries
#define I86_GDT_ENTRY_COUNT			5

// gdt code and data descriptor flags
#define I86_GDT_DESC_ACCESS			0x01			//00000001 - set by cpu when loaded into segment selector
#define I86_GDT_DESC_READWRITE		0x02			//00000010 - read/write
#define I86_GDT_DESC_EXPANSION		0x04			//00000100 - expansion direction
#define I86_GDT_DESC_EXEC_CODE		0x08			//00001000 - code or data descriptor

// system descriptor types
#define I86_GDT_SYSDESC_TSS16       0x01
#define I86_GDT_SYSDESC_LDT         0x02
#define I86_GDT_SYSDESC_CALLGATE16  0x04
#define I86_GDT_SYSDESC_TASKGATE    0x05
#define I86_GDT_SYSDESC_INT16       0x06
#define I86_GDT_SYSDESC_TRAP16      0x07
#define I86_GDT_SYSDESC_TSS32       0x09
#define I86_GDT_SYSDESC_CALLGATE32  0x0C
#define I86_GDT_SYSDESC_INT32       0x0E
#define I86_GDT_SYSDESC_TRAP32      0x0F

// general segment flags
#define I86_GDT_DESC_CODEDATA		0x10			//00010000 - code/data or system descriptor
#define I86_GDT_DESC_RING3			0x60			//01100000 - ring3 access
#define I86_GDT_DESC_RING2			0x40			//01000000 - ring2 access
#define I86_GDT_DESC_RING1			0x20			//00100000 - ring1 access
#define I86_GDT_DESC_RING0			0x00			//00000000 - ring0 access
#define I86_GDT_DESC_PRESENT		0x80			//10000000 - is segment available for use?

// gdt descriptor granularity bit flags
#define I86_GDT_GRAND_LIMITHI_MASK	0x0f			//00001111
#define I86_GDT_GRAND_AVAILABLE	    0x10			//00010000 - available for OS use
#define I86_GDT_GRAND_64BIT         0x20            //00100000 - reserved in 32 bit mode
#define I86_GDT_GRAND_32BIT			0x40			//01000000 - 16 or 32 bit segment
#define I86_GDT_GRAND_4K			0x80			//10000000 - scaling of limit field (4K or 1B)

//gdt descriptor offsets
#define GDT_KERNEL_CODE             0x08
#define GDT_KERNEL_DATA             0x10

struct gdt_entry
{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr
{
	uint16_t size;
	uint32_t offset;
} __attribute__((packed));


extern void gdt_flush(void);

void gdt_set_descriptor(uint16_t i, uint32_t base, uint32_t limit, uint8_t access, uint8_t grand);
void gdt_clear_tss_busy_flag(uint16_t i);
void gdt_install(void);

#endif
