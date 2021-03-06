GDB_VERSION    := 7.2
GDB_SOURCE     := $(TOOLCHAIN_SRCDIR)/gdb-$(GDB_VERSION).tar.bz2
GDB_DOWNLOAD   := http://ftp.gnu.org/gnu/gdb/gdb-$(GDB_VERSION).tar.bz2
GDB_PATCHES    := 

#Fixup GDB build for x86_64-elf
ifeq ($(TOOLCHAIN_TARGET),x86_64-elf)
	GDB_TARGET_ARCH := x86_64-pc-linux-gnu
else
	GDB_TARGET_ARCH = $(TOOLCHAIN_TARGET)
endif

# Download
$(GDB_SOURCE):
	$(call target_mkdir)
	$(call cmd_msg,WGET,$(subst $(SRC)/,,$(@)))
	$(Q)wget -c -O $(@).part $(GDB_DOWNLOAD)
	$(Q)mv $(@).part $(@)


# Extract
$(TOOLCHAIN_ROOTDIR)/.gdb-extract: $(GDB_SOURCE)
	$(Q)mkdir -p $(TOOLCHAIN_BUILDDIR)
	$(call cmd_msg,EXTRACT,$(subst $(SRC)/$(SRCSUBDIR)/,,$(GDB_SOURCE)))
	$(Q)tar -C $(TOOLCHAIN_BUILDDIR) -xjf $(GDB_SOURCE)
	$(call cmd_msg,PATCH,$(subst $(SRC)/$(SRCSUBDIR)/,,$(GDB_PATCHES)))
	$(Q)$(foreach patch,$(GDB_PATCHES), \
		cd $(TOOLCHAIN_BUILDDIR)/gdb-$(GDB_VERSION); \
		patch -Np1 -i $(patch) $(QOUTPUT); \
	)
	$(Q)touch $(@)


# Configure
$(TOOLCHAIN_ROOTDIR)/.gdb-configure: $(TOOLCHAIN_ROOTDIR)/.gdb-extract
	$(Q)if [ -d "$(TOOLCHAIN_BUILDDIR)/gdb-build" ]; then \
		rm -rf $(TOOLCHAIN_BUILDDIR)/gdb-build; \
	fi
	$(Q)mkdir -p $(TOOLCHAIN_BUILDDIR)/gdb-build
	$(call cmd_msg,CONFIG,$(TOOLCHAIN_TARGET)/gdb-$(GDB_VERSION) ($(GDB_TARGET_ARCH)))
	$(Q)cd $(TOOLCHAIN_BUILDDIR)/gdb-build; \
		../gdb-$(GDB_VERSION)/configure \
		--disable-werror \
		--prefix=$(TOOLCHAIN_ROOTDIR) \
		--target=$(GDB_TARGET_ARCH) \
		$(QOUTPUT)
	$(Q)touch $(@)


# Compile
$(TOOLCHAIN_ROOTDIR)/.gdb-compile: $(TOOLCHAIN_ROOTDIR)/.gdb-configure
	$(call cmd_msg,COMPILE,$(TOOLCHAIN_TARGET)/gdb-$(GDB_VERSION) ($(GDB_TARGET_ARCH)))
	$(Q)cd $(TOOLCHAIN_BUILDDIR)/gdb-build; $(MAKE) all $(QOUTPUT)
	$(Q)touch $(@)


# Install
$(TOOLCHAIN_ROOTDIR)/.gdb-install: $(TOOLCHAIN_ROOTDIR)/.gdb-compile
	$(call cmd_msg,INSTALL,$(TOOLCHAIN_TARGET)/gdb-$(GDB_VERSION) ($(GDB_TARGET_ARCH)))
	$(Q)cd $(TOOLCHAIN_BUILDDIR)/gdb-build; $(MAKE) install $(QOUTPUT)
	$(Q)touch $(@)



# Download, build and install gdb to run on the host system.
GDB_TARGET := $(TOOLCHAIN_ROOTDIR)/.gdb-install
all-gdb: $(GDB_TARGET)
.PHONY: all-gdb

all: $(GDB_TARGET)
download: $(GDB_SOURCE)
