# Kernelload.mk
# Created by Matheus Leme Da Silva

KERNEL_ELF := $(basename $(KERNEL)).elf

KERNEL_CFLAGS := -std=c11 -m32 -O2 \
					-ffreestanding -fno-stack-protector \
					-Wall -Wextra \
					-mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone
KERNEL_LDFLAGS := -melf_i386 -T$(SOURCEDIR)/Kernel/Kernel.ld

KERNEL_SOURCE := \
					Entry.asm \
					Vga.c \
					Gdt.c \
					Main.c
KERNEL_SOURCE := $(addprefix $(KERNEL_SOURCEDIR)/Kernel/,$(KERNEL_SOURCE))

KERNEL_SOURCE_C := $(filter %.c,$(KERNEL_SOURCE))
KERNEL_SOURCE_ASM := $(filter %.asm,$(KERNEL_SOURCE))

KERNEL_OBJ_C := $(patsubst $(KERNEL_SOURCEDIR)/Kernel/%.c,$(OBJDIR)/Kernel/C/%.o,$(KERNEL_SOURCE_C))
KERNEL_OBJ_ASM := $(patsubst $(KERNEL_SOURCEDIR)/Kernel/%.asm,$(OBJDIR)/Kernel/Asm/%.o,$(KERNEL_SOURCE_ASM))

KERNEL_OBJ := $(KERNEL_OBJ_C) $(KERNEL_OBJ_ASM)

KERNEL_DEP_C := $(patsubst $(KERNEL_SOURCEDIR)/Kernel/%.c,$(DEPDIR)/Kernel/C/%.d,$(KERNEL_SOURCE_C))
KERNEL_DEP_ASM := $(patsubst $(KERNEL_SOURCEDIR)/Kernel/%.asm,$(DEPDIR)/Kernel/Asm/%.d,$(KERNEL_SOURCE_ASM))

KERNEL_DEP := $(KERNEL_DEP_C) $(KERNEL_DEP_ASM)

$(KERNEL): $(KERNEL_ELF)
	mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@

$(KERNEL_ELF): $(KERNEL_OBJ)
	mkdir -p $(dir $@)
	$(LD) $(KERNEL_LDFLAGS) -o $@ $^

$(OBJDIR)/Kernel/C/%.o: $(SOURCEDIR)/Kernel/%.c
	mkdir -p $(dir $@) $(dir $(DEPDIR)/Kernel/C/$*)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@ -MMD -MF $(DEPDIR)/Kernel/C/$*.d

$(OBJDIR)/Kernel/Asm/%.o: $(SOURCEDIR)/Kernel/%.asm
	mkdir -p $(dir $@) $(dir $(DEPDIR)/Kernel/Asm/$*)
	$(NASM) -f elf32 $< -o $@ -MD -MF $(DEPDIR)/Kernel/Asm/$*.d

-include $(KERNEL_DEP)
