# include this in your main file if you want to copy
# the $TARGET onto a hdd image file after building


#"hook" the main targets here
all: diskimage
    @:	#Shut up!

clean: diskimageclean
    @:  #Shut up!


# Floppy
diskimage: hdd.img copykernel

hdd.img: $(TARGET)
	$(call cmd_msg,GUNZIP,hdd.img)
	$(Q)gunzip -c resources/hdd.img.gz > hdd.img

copykernel: $(TARGET)
	$(call cmd_msg,MOUNT,hdd.img)
	$(Q)sudo mkdir tmp

ifeq ($(shell uname),Darwin)
	$(Q)sudo hdiutil attach hdd.img -mountpoint tmp -readwrite $(QOUTPUT)
else
	$(Q)sudo mount -o loop,offset=32256 hdd.img tmp $(QOUTPUT)
endif

	$(call cmd_msg,CP,kernel.elf)
	$(Q)sudo cp kernel.elf tmp/kernel.elf

	$(call cmd_msg,UNMOUNT,hdd.img)
ifeq ($(shell uname),Darwin)
	$(Q)sudo hdiutil detach tmp $(QOUTPUT)
else
	$(Q)sudo umount tmp $(QOUTPUT)
endif

	$(Q)sudo rm -Rf tmp


qemu: all diskimage
	$(call cmd_msg,QEMU,hdd.img)
	$(Q)$(QEMU) -hda hdd.img


diskimageclean:
	$(Q)rm -f hdd.img


.PHONY: diskimage copykernel
