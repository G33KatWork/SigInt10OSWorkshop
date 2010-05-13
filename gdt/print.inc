%ifndef _PRINT_INCLUDED_
%define _PRINT_INCLUDED_

[BITS 16]

bios_print:
	lodsb                           ;Load byte at address DS:(E)SI into AL
	or      al, al	                ;or al with al to set or clear the zero flag
	jz      done                    ;check if last operation resulted in 0 and jump to done
	mov     ah, 0x0E                ;load 0x0E into ah -> tells bios what to do
	int     0x10                    ;call bios interrupt 0x10 (see http://en.wikipedia.org/wiki/BIOS_interrupt_call)
	jmp     bios_print              ;make a loop
done:
	ret                             ;we are done. return
    
%endif
