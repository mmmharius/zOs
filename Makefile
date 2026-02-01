ASM = nasm
CC = gcc
LD = ld

ASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -nostdlib -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld

all: kernel.bin

boot.o: boot/boot.asm
	$(ASM) $(ASMFLAGS) $< -o $@

kernel.o: src/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: boot.o kernel.o
	$(LD) $(LDFLAGS) -o $@ boot.o kernel.o

iso: kernel.bin
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/
	grub-mkrescue -o zOS.iso isodir

run: iso
	qemu-system-i386 -cdrom zOS.iso

clean:
	rm -f *.o kernel.bin

fclean: clean
	rm -rf isodir zOS.iso

re: fclean all

.PHONY: all iso run clean fclean re