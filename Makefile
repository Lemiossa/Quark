# Makefile
# Criado por Matheus Leme Da Silva
MAKEFLAGS += --no-print-directory

SOURCE_DIR := $(CURDIR)/source
BUILD_DIR := $(CURDIR)/build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj
DEP_DIR := $(BUILD_DIR)/dep

VERSION := 0.1
IMAGE_SIZE := 16M
IMAGE_FILE := $(BUILD_DIR)/quark.img

KERNEL := $(BIN_DIR)/kernel.bin
BOOTLOADER := $(BIN_DIR)/bootload.bin
CORE := $(BIN_DIR)/core.bin

BOOTLOADER_ELF := $(basename $(BOOTLOADER)).elf

KERNEL_ELF := $(basename $(KERNEL)).elf
KERNEL_SECTORS := $(basename $(KERNEL)).sct

SYSROOT := $(BUILD_DIR)/sysroot

SFDISK := /sbin/sfdisk
MKFS := /sbin/mkfs.fat
MCOPY := mcopy

CC := i686-elf-gcc
LD := i686-elf-ld
OBJCOPY := i686-elf-objcopy
GDB := i686-elf-gdb

LINKER := $(SOURCE_DIR)/link.ld

CFLAGS := -m32 -std=c11 -Os -g3 -Wall -Wextra -nostdinc -nostdlib -ffreestanding \
		  -fno-stack-protector -fno-stack-check -fno-lto -fno-PIC \
		  -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -fexec-charset=cp437
LDFLAGS := -T$(LINKER) -nostdlib -static
LIBS := -lgcc

SOURCE := \
		  entry.S \
			int.S \
			isr_table.S \
		  idt.c \
			pic.c \
			kmain.c \
			timer.c \
		  string.c \
		  terminal.c \
		  graphics.c \
		  panic.c \
			printk.c

SOURCE := $(addprefix $(SOURCE_DIR)/,$(SOURCE))

SOURCE_C := $(filter %.c,$(SOURCE))
SOURCE_S := $(filter %.S,$(SOURCE))

OBJ := \
	$(patsubst $(SOURCE_DIR)/%.S,$(OBJ_DIR)/%.S.o,$(SOURCE_S)) \
	$(patsubst $(SOURCE_DIR)/%.c,$(OBJ_DIR)/%.c.o,$(SOURCE_C))

DEP := \
	$(patsubst $(SOURCE_DIR)/%.S,$(DEP_DIR)/%.S.d,$(SOURCE_S)) \
	$(patsubst $(SOURCE_DIR)/%.c,$(DEP_DIR)/%.c.d,$(SOURCE_C))

all: $(IMAGE_FILE)

clean:
	@echo "  RM        $(BUILD_DIR)"
	@echo "  RM        $(IMAGE_FILE)"
	@echo "  RM        *.p*"

	@rm -rf $(BUILD_DIR) $(IMAGE_FILE) *.p*

$(CORE): $(KERNEL) $(BOOTLOADER)
	@echo "  CAT       $@"
	@cat $(BOOTLOADER) $(KERNEL) > $@

qemu: $(IMAGE_FILE)
	@echo "  QEMU      $<"
	@qemu-system-i386 -hda $< -m 16M -enable-kvm

dqemu: $(IMAGE_FILE)
	@echo "  QEMU      $(IMAGE_FILE)"
	@qemu-system-i386 \
		-drive format=raw,file=$(IMAGE_FILE) \
		-m 16M \
		-s -S \
		-no-reboot &

gdb: $(KERNEL_ELF)
	@echo "  GDB       .gdbinit"
	@echo "set architecture i386:intel" > .gdbinit
	@echo "set disassembly-flavor intel" >> .gdbinit
	@echo "add-symbol-file $(KERNEL_ELF) 0x7e00" >> .gdbinit
	@echo "target remote :1234" >> .gdbinit
	@gdb -x .gdbinit

$(BOOTLOADER_ELF): $(OBJ_DIR)/boot/bootsect.o
	@mkdir -p $(dir $@)
	@echo "  LD        $@"
	@$(LD) -melf_i386 -Ttext 0x7c00 -o $(BOOTLOADER_ELF) $^

$(BOOTLOADER): $(BOOTLOADER_ELF)
	@echo "  OBJCOPY   $@"
	@$(OBJCOPY) -O binary $< $@

$(OBJ_DIR)/boot/bootsect.o: $(SOURCE_DIR)/boot/bootsect.S $(KERNEL_SECTORS)
	@mkdir -p $(dir $@) $(DEP_DIR)
	@echo "  AS        $@"
	@$(CC) -DSECTORS_TO_LOAD=$(shell cat $(KERNEL_SECTORS)) -MMD -MF $(DEP_DIR)/bootsector.d -c $(SOURCE_DIR)/boot/bootsect.S -o $@

$(KERNEL_ELF): $(OBJ)
	@mkdir -p $(dir $@)
	@echo "  LD        $@"
	@$(CC) $(LDFLAGS) -o $(KERNEL_ELF) $^ $(LIBS)

$(KERNEL): $(KERNEL_ELF)
	@echo "  OBJCOPY   $@"
	@$(OBJCOPY) -O binary $< $@

$(KERNEL_SECTORS): $(KERNEL)
	@SIZE=$$(stat -c %s $<); \
		SECTORS=$$(( ($$SIZE + 511) / 512 )); \
		if [ ! -f $@ ] || [ "$$(cat $@)" -ne "$$SECTORS" ]; then \
			echo $$SECTORS > $@; \
		fi; \
		truncate -s $$(( $$SECTORS * 512 )) $<

$(OBJ_DIR)/%.c.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*)
	@echo "  CC        $@"
	@$(CC) $(CFLAGS) -c $< -o $@ -MMD -MF $(DEP_DIR)/$*.c.d

$(OBJ_DIR)/%.S.o: $(SOURCE_DIR)/%.S
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*)
	@echo "  AS        $@"
	@$(CC) $(CFLAGS) -c $< -o $@ -MMD -MF $(DEP_DIR)/$*.S.d

$(SYSROOT):
	@echo "  SYSROOT   $@"
	@mkdir -p $@
	@mkdir -p $@/QUARK
	@cp $(SOURCE_DIR) $@/QUARK -r
	@cp Makefile $@/QUARK
	@cp README.md $@/QUARK
	@cp LICENSE $@/QUARK

$(IMAGE_FILE): $(CORE) $(SYSROOT)
	@echo "  PART      $@.p1"
	@touch $@.p1
	@truncate -s $(IMAGE_SIZE) $(basename $@).p1
	@echo "  MKFS      FAT16 $(basename $@).p1"
	@$(MKFS) -F 16 -n "QUARKOS" $(basename $@).p1 >/dev/null
	@echo "  MCOPY     $(SYSROOT) -> $(basename $@).p1"
	@$(MCOPY) -i $(basename $@).p1 -s $(SYSROOT)/* ::/
	@echo "  CAT       $@"
	@cat $(CORE) $(basename $@).p1 > $@
	@echo "  SFDISK    $@"
	@SECTORS_VAL=$$(cat $(KERNEL_SECTORS)); \
	 START_SECTOR=$$(($$SECTORS_VAL + 1)); \
	 echo "label: dos\n$$START_SECTOR,,0e,*" | $(SFDISK) $@ >/dev/null 2>&1

.PHONY: all clean gdb qemu dqemu
-include $(DEP) $(DEP_DIR)/bootsect.d
