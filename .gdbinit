target remote | qemu-system-i386 -hda /home/matheus/Projects/Quark/Quark.img -m 16M -gdb stdio -S -no-reboot
set architecture i386:intel
set disassembly-flavor intel
add-symbol-file /home/matheus/Projects/Quark/build/bin/kernel.elf 0x8000
