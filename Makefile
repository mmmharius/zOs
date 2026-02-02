ASM = nasm
CC = gcc
LD = ld

ASMFLAGS = -f elf32
CFLAGS = -m32 -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld

all: kernel.bin

boot.o: boot/boot.asm
	$(ASM) $(ASMFLAGS) $< -o $@

kernel.o: src/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: boot.o kernel.o
	$(LD) $(LDFLAGS) -o $@ boot.o kernel.o

iso: kernel.bin
	grub-mkrescue -o zOS.iso isodir
	cp kernel.bin isodir/boot/

run: iso
	qemu-system-i386 -cdrom zOS.iso

clean:
	rm -f *.o kernel.bin

fclean: clean
	rm -rf zOS.iso

re: fclean all

.PHONY: all iso run clean fclean re