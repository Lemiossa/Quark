# Bootload.mk
# Created by Matheus Leme Da Silva

BOOTSECT := $(BINDIR)/Boot/Bootsect.bin
STAGE2 := $(BINDIR)/Boot/Stage2.bin
STAGE2_ELF := $(BINDIR)/Boot/Stage2.elf

BOOTLOADER_CFLAGS := -std=c11 -m32 -O0 \
					-ffreestanding -fno-stack-protector \
					-Wall -Wextra \
					-mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone
BOOTLOADER_LDFLAGS := -melf_i386 -T$(SOURCEDIR)/Boot/Bootload.ld

BOOTLOADER_SOURCE := \
					Entry.asm \
					Disk.asm \
					E820.asm \
					Util.c \
					Vga.c \
					Fat.c \
					Main.c

BOOTLOADER_SOURCE := $(addprefix $(SOURCEDIR)/Boot/,$(BOOTLOADER_SOURCE))

BOOTLOADER_SOURCE_C := $(filter %.c,$(BOOTLOADER_SOURCE))
BOOTLOADER_SOURCE_ASM := $(filter %.asm,$(BOOTLOADER_SOURCE))

BOOTLOADER_OBJ_C := $(patsubst $(SOURCEDIR)/Boot/%.c,$(OBJDIR)/Boot/C/%.o,$(BOOTLOADER_SOURCE_C))
BOOTLOADER_OBJ_ASM := $(patsubst $(SOURCEDIR)/Boot/%.asm,$(OBJDIR)/Boot/Asm/%.o,$(BOOTLOADER_SOURCE_ASM))

BOOTLOADER_OBJ := $(BOOTLOADER_OBJ_C) $(BOOTLOADER_OBJ_ASM)

BOOTLOADER_DEP_C := $(patsubst $(SOURCEDIR)/Boot/%.c,$(DEPDIR)/Boot/C/%.d,$(BOOTLOADER_SOURCE_C))
BOOTLOADER_DEP_ASM := $(patsubst $(SOURCEDIR)/Boot/%.asm,$(DEPDIR)/Boot/Asm/%.d,$(BOOTLOADER_SOURCE_ASM))

BOOTLOADER_DEP := $(BOOTLOADER_DEP_C) $(BOOTLOADER_DEP_ASM)

$(BOOTLOADER): $(BOOTSECT) $(STAGE2)
	cat $^ > $@

$(BOOTSECT): $(SOURCEDIR)/Boot/Bootsect.asm
	mkdir -p $(dir $@)
	$(NASM) $< -f bin -o $@

$(STAGE2_ELF): $(BOOTLOADER_OBJ)
	mkdir -p $(dir $@)
	$(LD) $(BOOTLOADER_LDFLAGS) -o $@ $^

$(STAGE2): $(STAGE2_ELF)
	mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@

$(OBJDIR)/Boot/C/%.o: $(SOURCEDIR)/Boot/%.c
	mkdir -p $(dir $@) $(dir $(DEPDIR)/Boot/C/$*)
	$(CC) $(BOOTLOADER_CFLAGS) -c $< -o $@ -MMD -MF $(DEPDIR)/Boot/C/$*.d

$(OBJDIR)/Boot/Asm/%.o: $(SOURCEDIR)/Boot/%.asm
	mkdir -p $(dir $@) $(dir $(DEPDIR)/Boot/Asm/$*)
	$(NASM) -f elf32 $< -o $@ -MD -MF $(DEPDIR)/Boot/Asm/$*.d

-include $(BOOTLOADER_DEP)
