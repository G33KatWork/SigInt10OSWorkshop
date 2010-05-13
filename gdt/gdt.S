[BITS 16]
[ORG 0x7c00]                        ;our location will be 0:0x7c00 - the bios will copy us to this location
jmp entry

%include "a20.inc"
%include "gdt.inc"
%include "print.inc"

entry:
	xor     ax, ax                  ;clear ax
	mov     ds, ax                  ;set ds - means we set the data segment to segment 0
	
	mov     si, msg                 ;set si = beginning of message
	call    bios_print              ;call sub

	mov     si, msg2
	call    bios_print

	call    enableA20
	
	mov     si, msg3
	call    bios_print
		
	call    loadGDT

	mov     si, msg4
	call    bios_print
 
	cli                             ;disable interrupts, if we are in pmode, we will have an invalid IDT
	mov     eax, cr0                ;ste bit 0 in cr0 for pmode
	or      eax, 1
	mov     cr0, eax

	jmp     CODE_DESC:pModeEntry    ;Far jump to our new code descriptor

[BITS 32]
%include "pmodePrint.inc"

pModeEntry:
    mov		ax, DATA_DESC		    ;set data segments to data selector (0x10)
	mov		ds, ax
	mov		ss, ax
	mov		es, ax
	mov		esp, 90000h		        ;stack begins from 90000h
    
    call    ClearScreen32
    
    mov     ebx, msg5
    call    Puts32
    
hang:                               ;just loop to do nothing
	jmp     hang
	
	
;---------------------------- DATA ----------------------------

msg     db	'Hello Geekend!', 13, 10, 0
msg2    db  'Enabling A20 Gate', 13, 10, 0
msg3    db  'A20 Gate enabled', 13, 10, 0
msg4    db  'GDT loaded', 13, 10, 0
msg5    db  'Welcome to protected mode', 10, 0


times 510-($-$$) db 0
db 0x55
db 0xAA
