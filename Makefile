# Makefile
# Criado por Matheus Leme Da Silva
MAKEFLAGS += --no-print-directory

VERSION := 0.1
IMAGE_SIZE := 16M
IMAGE_FILE := $(CURDIR)/quarkos-i386-v$(VERSION).img

BUILD_DIR := $(CURDIR)/build
QUARKFS_DIR = $(BUILD_DIR)/bin/quarkfs

BOOTLOADER := $(BUILD_DIR)/bin/bootloader/bootloader.bin

KERNEL := $(BUILD_DIR)/bin/kernel/kernel.bin
KERNEL_SIZE := $(BUILD_DIR)/bin/kernel/kernel.bin.sectors

# Ferramentas
SFDISK := /sbin/sfdisk
MKFS := /sbin/mkfs.fat

all: $(IMAGE_FILE)

clean:
	@echo "  RM        $(BUILD_DIR)"
	@echo "  RM        $(IMAGE_FILE)"
	@echo "  RM        *.p*"

	@rm -rf $(BUILD_DIR) $(IMAGE_FILE) *.p*

kernel:
	@echo "  MAKE      $(CURDIR)/kernel"
	@$(MAKE) -C $(CURDIR)/kernel TARGET_DIR=$(BUILD_DIR) TARGET=$(notdir $(KERNEL))

bootloader: kernel
	@echo "  MAKE      $(CURDIR)/bootloader"
	@$(MAKE) -C $(CURDIR)/bootloader TARGET_DIR=$(BUILD_DIR) TARGET=$(notdir $(BOOTLOADER)) TO_LOAD=$(shell cat $(KERNEL_SIZE))

qemu: $(IMAGE_FILE)
	@echo "  QEMU      HDA=$< M=16"
	@qemu-system-i386 -hda $< -m 16

dqemu: $(IMAGE_FILE)
	@echo "  GDB       .gdbinit"
	@echo "target remote | qemu-system-i386 -hda $(IMAGE_FILE) -m 16 -gdb stdio -S" > .gdbinit
	@echo "set architecture i386:intel" >> .gdbinit
	@echo "set disassembly-flavor intel" >> .gdbinit
	@echo "add-symbol-file $(KERNEL).elf 0x8000" >> .gdbinit
	@echo "layout asm" >> .gdbinit
	@echo "layout regs" >> .gdbinit
	@gdb -x .gdbinit

$(IMAGE_FILE): bootloader
	@echo "  PART      $@.p1"
	@touch $@.p1
	@truncate -s $(IMAGE_SIZE) $@.p1
	@echo "  MKFS      FAT16 $@.p1"
	@$(MKFS) -F 16 -n "QUARKOS" $@.p1 >/dev/null
	@echo "  CAT       $@"
	@cat $(BOOTLOADER) $(KERNEL) $@.p1 > $@
	@echo "  SFDISK    $@"
	@echo "label: dos\n$(shell expr $(shell cat $(KERNEL_SIZE)) + 1),$(IMAGE_SIZE),0e,*\n" | $(SFDISK) $@ >/dev/null 2>&1

.PHONY: all clean kernel bootloader qemu dqemu
