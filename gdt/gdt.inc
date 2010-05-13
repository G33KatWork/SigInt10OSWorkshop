%ifndef _GDT_INCLUDED_
%define _GDT_INCLUDED_

[BITS 16]

loadGDT:
    cli                             ;disable interrupts
    pusha
    lgdt    [toc]                   ;load gdt
    sti                             ;enable interrupts
    popa
    ret

;---------------------------- GDT ----------------------------

gdt_data: 
	dd 0                            ;null descriptor
	dd 0 

; gdt code:	                        ;code descriptor
	dw 0FFFFh                       ;limit low
	dw 0                            ;base low
	db 0                            ;base middle
	db 10011010b                    ;access
	db 11001111b                    ;granularity
	db 0                            ;base high

; gdt data:	                        ;data descriptor
	dw 0FFFFh                       ;limit low
	dw 0                            ;base low
	db 0                            ;base middle
	db 10010010b                    ;access
	db 11001111b                    ;granularity
	db 0                            ;base high

end_of_gdt:
toc: 
	dw end_of_gdt - gdt_data - 1 	;limit (Size of GDT)
	dd gdt_data 			        ;base of GDT
	
%define NULL_DESC 0
%define CODE_DESC 0x8
%define DATA_DESC 0x10
	
%endif