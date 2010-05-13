[ORG 0x7c00]                        ;our location will be 0:0x7c00 - the bios will copy us to this location
	xor     ax, ax                  ;clear ax
	mov     ds, ax                  ;set ds - means we set the data segment to segment 0
	
	mov     si, msg                 ;set si = beginning of message
	call    bios_print              ;call sub
	
	mov     si, msg2
	call    bios_print
	
	call    enableA20
	
	mov     si, msg3
	call    bios_print

hang:                               ;just loop to do nothing
	jmp     hang

bios_print:
	lodsb                           ;Load byte at address DS:(E)SI into AL
	or      al, al	                ;or al with al to set or clear the zero flag
	jz      done                    ;check if last operation resulted in 0 and jump to done
	mov     ah, 0x0E                ;load 0x0E into ah -> tells bios what to do
	int     0x10                    ;call bios interrupt 0x10 (see http://en.wikipedia.org/wiki/BIOS_interrupt_call)
	jmp     bios_print              ;make a loop
done:
	ret                             ;we are done. return
	
enableA20:
    cli
	push    ax
	mov	    al, 0xdd	            ;send enable a20 address line command to controller
	out	    0x64, al
	pop	    ax
	sti
    ret
	
    msg     db	'Hello Geekend!', 13, 10, 0
    msg2    db  'Enabling A20 Gate', 13, 10, 0
    msg3    db  'A20 Gate enabled', 13, 10, 0

    times 510-($-$$) db 0
    db 0x55
    db 0xAA
