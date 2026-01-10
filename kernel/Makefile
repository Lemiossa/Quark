# Makefile
# Criado por Matheus Leme Da Silva

TARGET_DIR ?= $(CURDIR)/build
TARGET ?= kernel.bin

SRC_DIR := $(CURDIR)/src
BIN_DIR := $(TARGET_DIR)/bin/kernel
OBJ_DIR := $(TARGET_DIR)/obj/kernel
DEP_DIR := $(TARGET_DIR)/dep/kernel

LINKER := $(CURDIR)/link.ld

CC ?= gcc
LD ?= ld
OBJCOPY ?= objcopy

CFLAGS := -m32 -std=c99 -Os -g3 -Wall -Wextra -nostdinc -nostdlib -ffreestanding \
		  -fno-stack-protector -fno-stack-check -fno-lto -fno-PIC \
		  -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -fexec-charset=cp437 \
			-ffunction-sections -fdata-sections

LDFLAGS := -melf_i386 -T$(LINKER) -nostdlib -static --gc-sections -s

TARGET_FILE := $(BIN_DIR)/$(notdir $(TARGET))

SRC := \
		  entry.S \
			interrupts.S \
		  idt.c \
			pic.c \
			kmain.c \
		  string.c \
		  terminal/terminal.c \
		  terminal/char.c \
		  graphics.c \
		  panic.c

SRC := $(addprefix $(SRC_DIR)/,$(SRC))

SRC_C := $(filter %.c,$(SRC))
SRC_S := $(filter %.S,$(SRC))

OBJ := \
	$(patsubst $(SRC_DIR)/%.S,$(OBJ_DIR)/%.S.o,$(SRC_S)) \
	$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.c.o,$(SRC_C))

DEP := \
	$(patsubst $(SRC_DIR)/%.S,$(DEP_DIR)/%.S.d,$(SRC_S)) \
	$(patsubst $(SRC_DIR)/%.c,$(DEP_DIR)/%.c.d,$(SRC_C))


all: $(TARGET_FILE)

clean:
	@echo "  RM        $(TARGET_DIR)"
	@rm -rf $(TARGET_DIR)

$(TARGET_FILE): $(OBJ)
	@mkdir -p $(dir $@)
	@echo "  LD        $@.elf"
	@$(LD) $(LDFLAGS) -o $@.elf $^
	@echo "  OBJCOPY   $@"
	@$(OBJCOPY) -O binary $@.elf $@
	@SIZE=$$(stat -c %s $@); \
		SECTORS=$$(( ($$SIZE + 511) / 512 )); \
		echo $$SECTORS> $@.sectors; \
		truncate -s $$(( $$SECTORS * 512 )) $@

$(OBJ_DIR)/%.c.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*)
	@echo "  CC        $@"
	@$(CC) $(CFLAGS) -c $< -o $@ -MMD -MF $(DEP_DIR)/$*.c.d

$(OBJ_DIR)/%.S.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*)
	@echo "  AS        $@"
	@$(CC) $(CFLAGS) -c $< -o $@ -MMD -MF $(DEP_DIR)/$*.S.d

.PHONY: all clean
-include $(DEP)
