SRC                 := .
include $(SRC)/build/base.mak

STARTTIME := $(shell date +%s)

SUBTARGETS = a20gate biosprint gdt cboot interrupts longmode timer
DIRS = $(SUBTARGETS:%=%.dir)

# Main targets
all: toolchain $(DIRS)
	$(call cmd_msg,NOTICE,Build completed in $$(($$(date +%s)-$(STARTTIME))) seconds)

%.dir:
	$(call cmd_msg,SUBDIR,$*)
	$(call call_submake,$*,all)
	@echo ""

# Toolchain
toolchain: $(CC)

$(CC):
	$(call cmd_msg,SUBDIR,toolchain)
	$(call call_submake,toolchain,all)

# Cleaning
clean:
	@-for i in $(SUBTARGETS); do \
		$(MAKE) -C $$i clean; \
	done
	$(call call_submake,toolchain,clean)

distclean: clean
	@-for i in $(SUBTARGETS); do \
		$(MAKE) -C $$i distclean; \
	done
	$(call call_submake,toolchain,distclean)
	$(call call_submake,toolchain,toolchain-clean)

.PHONY: all subbuild subclean subdistclean clean distclean

