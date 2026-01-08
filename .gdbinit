target remote | qemu-system-i386 -hda /home/matheus/Projects/Quark/quarkos-i386-v0.1.img -m 16 -gdb stdio -S
set architecture i386:intel
set disassembly-flavor intel
add-symbol-file build/bin/kernel/kernel.bin.elf 0x8000
layout asm
layout regs
