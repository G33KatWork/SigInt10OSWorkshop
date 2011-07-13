# include this in your main makefile and overwrite CFLAGS,
# CXXFLAGS, LDFLAGS, ASOURCES, GASOURCES, CCSOURCES and
# CXXSOURCES if you want to build C/C++ sourcecode into
# and ELF file

# If Clang is used, the host triple needs to be defined and the internal assembler activated
ifeq ($(USE_LLVM),1)
CFLAGS		+= -ccc-host-triple x86_64-pc-elf -integrated-as
CXXFLAGS 	+= -ccc-host-triple x86_64-pc-elf -integrated-as
endif

# Determinte objects to be created
OBJECTS += $(ASOURCES:%.S=%.o)
OBJECTS += $(GASOURCES:%.s=%.o)
OBJECTS += $(CXXSOURCES:%.cpp=%.o)
OBJECTS += $(CCSOURCES:%.c=%.o)


# Main targets
all: createdirs $(TARGET)
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


# Cleaning
clean:
	$(Q)rm -f $(TARGET)
#$(Q)rm -f $(foreach file,$(patsubst %.o,%,$(OBJECTS)),$(foreach ext,o,$(file).$(ext)))
	$(Q)rm -f source
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


.PHONY: clean distclean
