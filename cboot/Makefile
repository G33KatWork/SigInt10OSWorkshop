SRC                 := ..
include $(SRC)/build/base.mak

# Target file name.
TARGET = kernel.elf

# List C source files here.
CCSOURCES = main.c

# List C++ source files here
CXXSOURCES =

# List Assembler to be assembled with NASM here
ASOURCES = start.S

# Additional include paths to consider
INCLUDES =	include

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# C compiler flags
CFLAGS += -g -nostdlib -nostdinc -fno-builtin -fno-stack-protector -std=gnu99
CFLAGS += -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
			-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
			-Wstrict-prototypes #-Wconversion
CFLAGS += -DARCH_X86 -DDEBUG
CFLAGS += $(addprefix -I,$(INCLUDES))

# C++ compiler flags
CXXFLAGS += -g -fno-builtin -fno-exceptions -fno-rtti -nostdlib -nostartfiles -nodefaultlibs
CXXFLAGS += -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wredundant-decls -Winline -Wno-long-long \
			-Wconversion #-Werror
CXXFLAGS += -DARCH_X86 -DDEBUG
CXXFLAGS += $(addprefix -I,$(INCLUDES))

# NASM flags
ASFLAGS = -f elf -g

# Linker flags
LDFLAGS = -T linker.ld -Map kernel.map

# Determinte objects to be created
OBJECTS += $(ASOURCES:%.S=%.o)
OBJECTS += $(GASOURCES:%.s=%.o)
OBJECTS += $(CXXSOURCES:%.cpp=%.o)
OBJECTS += $(CCSOURCES:%.c=%.o)

# Main targets
all: createdirs $(TARGET) floppy.img
	@:	#Shut up!

# sort for removing duplicates
$(sort $(addprefix $(OBJDIR)/, $(dir $(OBJECTS)))):
	$(call cmd_msg,MKDIR,$(@))
	$(Q)mkdir -p $@

createdirs: $(sort $(addprefix $(OBJDIR)/, $(dir $(OBJECTS))))

$(TARGET): $(addprefix $(OBJDIR)/, $(OBJECTS))
	$(call cmd_msg,LINK,$(@))
	$(Q)$(LD) $(LDFLAGS) -o $@ $^
	$(call cmd_msg,OBJDUMP,$(@) -> source)
	$(Q)$(OBJDUMP) --source $(TARGET) > source

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

	$(Q)sudo cp kernel.elf tmp/kernel

ifeq ($(shell uname),Darwin)
	$(Q)sudo hdiutil detach tmp $(QOUTPUT)
else
	$(Q)sudo umount tmp $(QOUTPUT)
endif

	$(Q)sudo rm -Rf tmp

# Cleaning
clean:
	$(Q)rm -f $(TARGET)
#$(Q)rm -f $(foreach file,$(patsubst %.o,%,$(OBJECTS)),$(foreach ext,o,$(file).$(ext)))
	$(Q)rm -f source
	$(Q)rm -f floppy.img
	$(Q)rm -f kernel.map
	$(Q)rm -rf obj

distclean: clean

# Compile cpp files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(call cmd_msg,CXX,$<)
	$(Q)$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile c files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(call cmd_msg,CC,$<)
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

# Assembler S files
$(OBJDIR)/%.o: $(SRCDIR)/%.S
	$(call cmd_msg,NASM,$<)
	$(Q)$(NASM) $(ASFLAGS) -o $@ $<

.PHONY: clean distclean bootfloppy floppy.img
