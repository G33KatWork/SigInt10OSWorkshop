SRC                 := ..
include $(SRC)/build/base.mak

TARGET = gdt

all: $(TARGET).bin floppy

$(TARGET).bin: $(TARGET).S
	$(call cmd_msg,NASM,$<)
	$(Q)$(NASM) -f bin -o $@ $<

floppy:
	$(call cmd_msg,MKFLOPPY,floppy.img)
	$(Q)dd if=/dev/zero of=floppybla.img bs=512 count=2879 $(QOUTPUT)
	$(Q)cat $(TARGET).bin floppybla.img > floppy.img
	$(Q)rm floppybla.img

# Cleaning
clean:
	$(Q)rm -f $(TARGET).bin
	$(Q)rm -f floppy.img

distclean: clean

.PHONY: floppy clean distclean
