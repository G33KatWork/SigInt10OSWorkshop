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
CFLAGS += -g -nostdlib -nostdinc -nodefaultlibs -ffreestanding -fno-builtin -fno-stack-protector -std=gnu99 -mcmodel=large
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
ASFLAGS = -g -f elf64

# Linker flags
LDFLAGS = -T linker.ld -Map kernel.map -z max-page-size=0x1000

include $(SRC)/build/targets/c/cTarget.mak
include $(SRC)/build/targets/c/hddTarget.mak
