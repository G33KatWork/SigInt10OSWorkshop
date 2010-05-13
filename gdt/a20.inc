%ifndef _A20_INCLUDED_
%define _A20_INCLUDED_

[BITS 16]

enableA20:
    cli
	push    ax
	mov	    al, 0xdd	            ;send enable a20 address line command to controller
	out	    0x64, al
	pop	    ax
	sti
    ret

%endif