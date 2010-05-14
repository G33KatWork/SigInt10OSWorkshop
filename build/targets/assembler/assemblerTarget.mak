# include this in your base makefile and overwrite the
# variable TARGET with the main (and only) .S file you
# want to assemble and put on a floppy in the mbr

all: $(TARGET).bin floppy

$(TARGET).bin: $(TARGET).S
	$(call cmd_msg,NASM,$<)
	$(Q)$(NASM) -f bin -o $@ $<

floppy:
	$(call cmd_msg,MKFLOPPY,floppy.img)
	$(Q)dd if=/dev/zero of=floppybla.img bs=512 count=2879 $(QOUTPUT)
	$(Q)cat $(TARGET).bin floppybla.img > floppy.img
	$(Q)rm floppybla.img

qemu: floppy.img
	$(call cmd_msg,QEMU,floppy.img)
	$(Q)$(QEMU) -hda floppy.img

# Cleaning
clean:
	$(Q)rm -f $(TARGET).bin
	$(Q)rm -f floppy.img

distclean: clean

.PHONY: floppy clean distclean
