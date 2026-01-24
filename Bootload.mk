# Bootload.mk
# Created by Matheus Leme Da Silva

BOOTSECT := $(BINDIR)/Boot/Bootsect.bin
STAGE2 := $(BINDIR)/Boot/Stage2.bin
STAGE2_ELF := $(BINDIR)/Boot/Stage2.elf

CFLAGS := -std=c11 -m32 -Os \
					-ffreestanding -fno-stack-protector \
					-Wall -Wextra \
					-mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone
LDFLAGS := -melf_i386 -T$(SOURCEDIR)/Boot/Bootload.ld

SOURCE := \
					Entry.asm \
					Disk.asm \
					Util.c \
					Vga.c \
					Fat.c \
					Main.c
SOURCE := $(addprefix $(SOURCEDIR)/Boot/,$(SOURCE))

SOURCE_C := $(filter %.c,$(SOURCE))
SOURCE_ASM := $(filter %.asm,$(SOURCE))

OBJ_C := $(patsubst $(SOURCEDIR)/Boot/%.c,$(OBJDIR)/Boot/C/%.o,$(SOURCE_C))
OBJ_ASM := $(patsubst $(SOURCEDIR)/Boot/%.asm,$(OBJDIR)/Boot/Asm/%.o,$(SOURCE_ASM))

OBJ := $(OBJ_C) $(OBJ_ASM)

DEP_C := $(patsubst $(SOURCEDIR)/Boot/%.c,$(DEPDIR)/Boot/C/%.d,$(SOURCE_C))
DEP_ASM := $(patsubst $(SOURCEDIR)/Boot/%.asm,$(DEPDIR)/Boot/Asm/%.d,$(SOURCE_ASM))

DEP := $(DEP_C) $(DEP_ASM)

$(BOOTLOADER): $(BOOTSECT) $(STAGE2)
	cat $^ > $@

$(BOOTSECT): $(SOURCEDIR)/Boot/Bootsect.asm
	mkdir -p $(dir $@)
	$(NASM) $< -f bin -o $@

$(STAGE2_ELF): $(OBJ)
	mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^

$(STAGE2): $(STAGE2_ELF)
	mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@

$(OBJDIR)/Boot/C/%.o: $(SOURCEDIR)/Boot/%.c
	mkdir -p $(dir $@) $(dir $(DEPDIR)/Boot/C/$*)
	$(CC) $(CFLAGS) -c $< -o $@ -MMD -MF $(DEPDIR)/Boot/C/$*.d

$(OBJDIR)/Boot/Asm/%.o: $(SOURCEDIR)/Boot/%.asm
	mkdir -p $(dir $@) $(dir $(DEPDIR)/Boot/Asm/$*)
	$(NASM) -f elf32 $< -o $@ -MD -MF $(DEPDIR)/Boot/Asm/$*.d

-include $(DEP)
