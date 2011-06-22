# include this in your main file if you want to copy
# the $TARGET onto a floppy image file after building


#"hook" the main targets here
all: floppy.img
    @:	#Shut up!

clean: floppyclean
    @:  #Shut up!


# Floppy
bootfloppy: all floppy.img

floppy.img: $(TARGET)
	$(call cmd_msg,MKFLOPPY,floppy.img)
	$(Q)sudo mkdir tmp
	$(Q)sudo cp resources/floppy.fat.img ./floppy.img

ifeq ($(shell uname),Darwin)
	$(Q)sudo hdiutil attach floppy.img -mountpoint tmp -readwrite $(QOUTPUT)
else
	$(Q)sudo mount -o loop floppy.img tmp $(QOUTPUT)
endif

	$(Q)sudo cp $(TARGET) tmp/kernel

ifeq ($(shell uname),Darwin)
	$(Q)sudo hdiutil detach tmp $(QOUTPUT)
else
	$(Q)sudo umount tmp $(QOUTPUT)
endif

	$(Q)sudo chmod 666 floppy.img
	$(Q)sudo rm -Rf tmp


qemu: all floppy.img
	$(call cmd_msg,QEMU,floppy.img)
	$(Q)$(QEMU) -fda floppy.img

qemudebug: floppy.img
	$(call cmd_msg,QEMU,floppy.img)
	$(call cmd_msg,NOTE,Waiting for gdb attachment on port 1234...)
	$(Q)$(QEMU) -net none -fda floppy.img -serial file:serialOut -s -S $(QOUTPUT)

floppyclean:
	$(Q)rm -f floppy.img


.PHONY: bootfloppy floppy.img
