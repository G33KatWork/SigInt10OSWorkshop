OUTPUT_FORMAT(elf64-x86-64)
ENTRY (bootstrap)

SECTIONS{
    . = 0x100000;
    .bootstrap :
    {
        obj/start.o (.text)
    }
    
    
    . += 0x0;
    .text : AT(ADDR(.text) - 0x0)
    {
        _code = .;
        *(.text)
        *(.rodata)
        . = ALIGN(4096);
    }

    .data : AT(ADDR(.data) - 0x0)
    {
        _data = .;
        *(.data)
        . = ALIGN(4096);
    }
    
    .ehframe : AT(ADDR(.ehframe) - 0x0)
    {
        _ehframe = .;
        *(.ehframe)
        . = ALIGN(4096);
    }

    .bss : AT(ADDR(.data) - 0x0)
    {
        _bss = .;
        *(.bss)
        . = ALIGN(4096);
    }
    
    _end = .;
    
    /DISCARD/ :
    {
        *(.comment)
    }
}
