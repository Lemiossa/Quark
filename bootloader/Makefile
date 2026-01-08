# Makefile
# Criado por Matheus Leme Da Silva

TARGET_DIR ?= $(CURDIR)/build
TARGET ?= bootloader.bin
TO_LOAD ?= 63

SRC_DIR := $(CURDIR)/src
BIN_DIR := $(TARGET_DIR)/bin/bootloader
OBJ_DIR := $(TARGET_DIR)/obj/bootloader
DEP_DIR := $(TARGET_DIR)/dep/bootloader

CC ?= gcc
LD ?= ld
OBJCOPY ?= objcopy

LDFLAGS := -melf_i386 -Ttext 0x7c00
CFLAGS := -m16 -DSECTORS_TO_LOAD=$(TO_LOAD)

TARGET_FILE := $(BIN_DIR)/$(notdir $(TARGET))

OBJ := $(OBJ_DIR)/bootsector.o
DEP := $(DEP_DIR)/bootsector.d

all: $(TARGET_FILE)

clean:
	@echo "  RM        $(TARGET_DIR)"
	@rm -rf $(TARGET_DIR)

$(TARGET_FILE): $(BIN_DIR)/bootsector.bin
	@echo "  CAT       $@"
	@cat $^ > $@

$(BIN_DIR)/bootsector.bin: $(OBJ_DIR)/bootsector.o
	@mkdir -p $(dir $@)
	@echo "  LD        $@.elf"
	@$(LD) $(LDFLAGS) -o $@.elf $^
	@echo "  OBJCOPY   $@"
	@$(OBJCOPY) -O binary $@.elf $@

$(OBJ_DIR)/bootsector.o: $(SRC_DIR)/bootsector.S
	@mkdir -p $(dir $@) $(DEP_DIR)
	@echo "  CC        $@"
	@$(CC) $(CFLAGS) -MMD -MF $(DEP_DIR)/bootsector.d -c $< -o $@

.PHONY: all clean
-include $(DEP)
