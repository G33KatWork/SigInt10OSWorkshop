PAGE_SIZE	equ	0x1000
STACK_SIZE	equ	16*1024

PML2		equ	0x200000
PML3		equ	PML2+PAGE_SIZE
PML4		equ	PML3+PAGE_SIZE

;---------------------------------------------------------------------------------------------
MODULEALIGN        equ        1<<0
MEMINFO            equ        1<<1
FLAGS              equ        MODULEALIGN | MEMINFO
MAGIC              equ        0x1BADB002
CHECKSUM           equ        -(MAGIC + FLAGS)

[extern main]
[global bootstrap]

;---------------------------------------------------------------------------------------------
section .text

[BITS 32]
align 4
MultiBootHeader:
	dd MAGIC
    dd FLAGS
    dd CHECKSUM
    
bootstrap:
    cli

    ; clear 3 pages of pagetables
    mov edi, PML2
    xor eax, eax
    mov ecx, 3*4096/4
    rep stosd

    ; set up pagetables
    mov dword [PML2], 0x87		; single 4 MB id mapping PML2
    mov dword [PML3], PML2 | 7	; single entry at PML3
    mov dword [PML4], PML3 | 7	; single entry at PML4

    ; load the GDT
    lgdt [gdt_desc]

    ; set PSE,  PAE
    mov eax, 0x30
    mov cr4, eax

    ; long mode
    mov ecx, 0xc0000080
    rdmsr
    or eax, 0x100
    wrmsr

    ; enable pagetables
    mov eax, PML4
    mov cr3, eax

    ; turn on long mode and paging
    mov eax, 0x80010001
    mov cr0, eax

    jmp SEL_CS:code64

[BITS 64]
code64:
    mov ax, SEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, bootStack
    call main

inf:
    jmp inf
    

;---------------------------------------------------------------------------------------------
section .data

gdt_desc:
	dw  GDT_LEN-1
	dd  gdt

align 8
gdt	    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ; 0x00 dummy
gdt_cs	db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9b, 0xaf, 0x00 ; 0x08 code64
gdt_ds	db 0xff, 0xff, 0x00, 0x00, 0x00, 0x93, 0xaf, 0x00 ; 0x18 data64

GDT_LEN equ $ - gdt
SEL_CS equ gdt_cs - gdt
SEL_DS equ gdt_ds - gdt

;---------------------------------------------------------------------------------------------
section .bss
align 32

global bootStack
bootStackEnd:
	resb      STACK_SIZE
bootStack:
