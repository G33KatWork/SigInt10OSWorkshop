LLVM_VERSION	:= 2.9
LLVM_SOURCE		:= $(TOOLCHAIN_SRCDIR)/llvm-$(LLVM_VERSION).tgz
LLVM_DOWNLOAD	:= http://llvm.org/releases/$(LLVM_VERSION)/llvm-$(LLVM_VERSION).tgz

CLANG_VERSION	:= 2.9
CLANG_SOURCE	:= $(TOOLCHAIN_SRCDIR)/clang-$(CLANG_VERSION).tgz
CLANG_DOWNLOAD	:= http://llvm.org/releases/$(CLANG_VERSION)/clang-$(CLANG_VERSION).tgz

PATH += :$(TOOLCHAIN_ROOTDIR)/bin

# Download
$(LLVM_SOURCE):
	$(call target_mkdir)
	$(call cmd_msg,WGET,$(subst $(SRC)/,,$(@)))
	$(Q)wget -c -O $(@).part $(LLVM_DOWNLOAD)
	$(Q)mv $(@).part $(@)

$(CLANG_SOURCE):
	$(call target_mkdir)
	$(call cmd_msg,WGET,$(subst $(SRC)/,,$(@)))
	$(Q)wget -c -O $(@).part $(CLANG_DOWNLOAD)
	$(Q)mv $(@).part $(@)

# Extract
$(TOOLCHAIN_ROOTDIR)/.llvm-extract: $(LLVM_SOURCE) $(CLANG_SOURCE)
	$(Q)mkdir -p $(TOOLCHAIN_BUILDDIR)
	$(call cmd_msg,EXTRACT,$(subst $(SRC)/$(SRCSUBDIR)/,,$(LLVM_SOURCE)))
	$(Q)tar -C $(TOOLCHAIN_BUILDDIR) -xjf $(LLVM_SOURCE)

	$(call cmd_msg,EXTRACT,$(subst $(SRC)/$(SRCSUBDIR)/,,$(CLANG_SOURCE)))
	$(Q)tar -C $(TOOLCHAIN_BUILDDIR)/llvm-$(LLVM_VERSION)/tools -xjf $(CLANG_SOURCE)
	$(Q)mv $(TOOLCHAIN_BUILDDIR)/llvm-$(LLVM_VERSION)/tools/clang-$(CLANG_VERSION) $(TOOLCHAIN_BUILDDIR)/llvm-$(LLVM_VERSION)/tools/clang
	$(Q)touch $(@)

# Configure
$(TOOLCHAIN_ROOTDIR)/.llvm-configure: $(TOOLCHAIN_ROOTDIR)/.llvm-extract
	$(Q)if [ -d "$(TOOLCHAIN_BUILDDIR)/llvm-build" ]; then \
		rm -rf $(TOOLCHAIN_BUILDDIR)/llvm-build; \
	fi
	$(Q)mkdir -p $(TOOLCHAIN_BUILDDIR)/llvm-build
	$(call cmd_msg,CONFIG,$(TOOLCHAIN_TARGET)/llvm-$(LLVM_VERSION) ($(TOOLCHAIN_TARGET)))
	$(Q)cd $(TOOLCHAIN_BUILDDIR)/llvm-build; \
		../llvm-$(LLVM_VERSION)/configure \
			--prefix=$(TOOLCHAIN_ROOTDIR) \
			--target=$(TOOLCHAIN_TARGET) \
			$(QOUTPUT)
	$(Q)touch  $(@)


# Compile
$(TOOLCHAIN_ROOTDIR)/.llvm-compile: $(TOOLCHAIN_ROOTDIR)/.llvm-configure
	$(call cmd_msg,COMPILE,$(TOOLCHAIN_TARGET)/llvm-$(LLVM_VERSION) ($(TOOLCHAIN_TARGET)))
	$(Q)cd $(TOOLCHAIN_BUILDDIR)/llvm-build; \
		$(MAKE) all $(QOUTPUT)
	$(Q)touch $(@)


# Install
$(TOOLCHAIN_ROOTDIR)/.llvm-install: $(TOOLCHAIN_ROOTDIR)/.llvm-compile
	$(call cmd_msg,INSTALL,$(TOOLCHAIN_TARGET)/llvm-$(LLVM_VERSION) ($(TOOLCHAIN_TARGET)))
	$(Q)cd $(TOOLCHAIN_BUILDDIR)/llvm-build; $(MAKE) install $(QOUTPUT)
	$(Q)touch $(@)



# Download, build and install binutils to run on the host system.
LLVM_TARGET := $(TOOLCHAIN_ROOTDIR)/.llvm-install
all-llvm: $(BINUTILS_TARGET)
.PHONY: all-llvm

all: $(LLVM_TARGET)
download: $(LLVM_SOURCE) $(CLANG_SOURCE)
