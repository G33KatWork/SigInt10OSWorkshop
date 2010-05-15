[BITS 32]
global start
extern kmain

MODULEALIGN        equ        1<<0
MEMINFO            equ        1<<1
FLAGS              equ        MODULEALIGN | MEMINFO
MAGIC              equ        0x1BADB002
CHECKSUM           equ        -(MAGIC + FLAGS)

section .text                   ; Next is the Grub Multiboot Header

align 4
MultiBootHeader:
	dd MAGIC
    dd FLAGS
    dd CHECKSUM

STACKSIZE equ 0x4000            ; 16k if you're wondering

start:
	mov esp, bootStack + STACKSIZE	; Setup stack
	mov ebp, esp
	push eax                    ; Push magic number from GRUB
	push ebx					; Push pointer to GRUB multiboot-information
	
	call kmain					; Jump to C-Code
	cli

cpuhalt:
    hlt
    jmp cpuhalt

;---------------------------------------------------------------------------------------------
section .bss
align 32

global bootStack
bootStack:
	resb      STACKSIZE