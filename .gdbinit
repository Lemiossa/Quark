set architecture i386:intel
set disassembly-flavor intel
add-symbol-file /home/matheus/Projects/Quark/build/bin/kernel.elf 0x7e00
target remote :1234
