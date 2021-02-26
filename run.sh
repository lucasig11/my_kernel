mkdir -p build bin

# assemble boot.s file
as --32 boot.S -o build/boot.o

# compile C files
gcc -m32 -c source/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

gcc -m32 -c source/utils.c -o build/utils.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

gcc -m32 -c source/char.c -o build/char.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# linking the kernel with object files
ld -m elf_i386 -T linker.ld build/kernel.o build/char.o build/utils.o build/boot.o -o bin/LCOS.bin -nostdlib

# check if the os bin file is x86 multiboot or not
grub-file --is-x86-multiboot bin/LCOS.bin

# build the ISO file
mkdir -p isodir/boot/grub iso
cp bin/LCOS.bin isodir/boot/LCOS.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o iso/LCOS.iso isodir

# run in qemu
qemu-system-x86_64 -cdrom iso/LCOS.iso