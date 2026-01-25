# Makefile
# Created by Matheus Leme Da Silva

SOURCEDIR := $(CURDIR)/Source
BUILDDIR := $(CURDIR)/Build
BINDIR := $(BUILDDIR)/Bin
OBJDIR := $(BUILDDIR)/Obj
DEPDIR := $(BUILDDIR)/Dep

SFDISK := /sbin/sfdisk
NASM := nasm
QEMU := QEMU_AUDIO_DRV=pa qemu-system-i386
MKFS := /sbin/mkfs.fat
CC := i686-elf-gcc
LD := i686-elf-ld
OBJCOPY := i686-elf-objcopy

IMAGESIZE := 16M
IMAGEFAT := 16
IMAGE := $(BUILDDIR)/Quark.img

QEMU_FLAGS := -drive file=$(IMAGE),if=ide,format=raw,media=disk -m 16M -serial stdio
BOOTLOADER := $(BINDIR)/Bootload.bin
KERNEL := $(BINDIR)/Kernel.bin

.PHONY: all
all: $(IMAGE)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: qemu
qemu: $(IMAGE)
	$(QEMU) $(QEMU_FLAGS)

# IMAGE =======================================================================

IMAGEROOT := $(BUILDDIR)/IMGROOT

$(IMAGEROOT):
	mkdir -p $@
	mkdir -p $@/boot
	cp $(KERNEL) $@/boot/Kernel.sys

$(IMAGE): $(BOOTLOADER) $(KERNEL) $(IMAGEROOT)
	mkdir -p $(dir $@)
	dd if=/dev/zero of=$@ bs=512 count=63 status=progress
	dd if=$(BOOTLOADER) of=$@ bs=1 status=progress conv=notrunc
	dd if=/dev/zero of=$(basename $@).p1 bs=$(IMAGESIZE) count=1 status=progress
	$(MKFS) -F $(IMAGEFAT) -n "QUARK" $(basename $@).p1
	mcopy -i $(basename $@).p1 -s $(IMAGEROOT)/* "::/"
	dd if=$(basename $@).p1 >> $@
	echo "label:dos\n63,$(IMAGESIZE),0E,*\n" | $(SFDISK) $@

# BOOTLOADER ==================================================================

include Bootload.mk

# KERNEL ======================================================================

include Kernel.mk

