mkdir -p build/bin

# assemble boot.s file
as --32 boot.s -o build/boot.o

# compile C files
gcc -m32 -c source/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

gcc -m32 -c source/utils.c -o build/utils.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

gcc -m32 -c source/char.c -o build/char.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# linking the kernel with object files
ld -m elf_i386 -T linker.ld build/kernel.o build/char.o build/utils.o build/boot.o -o build/bin/LCOS.bin -nostdlib

# check if the os bin file is x86 multiboot or not
grub-file --is-x86-multiboot build/bin/LCOS.bin

# build the ISO file
mkdir -p build/isodir/boot/grub build/iso
cp build/bin/LCOS.bin build/isodir/boot/LCOS.bin
cp grub.cfg build/isodir/boot/grub/grub.cfg
grub-mkrescue -o build/iso/LCOS.iso build/isodir

# run in qemu
qemu-system-x86_64 -cdrom build/iso/LCOS.iso