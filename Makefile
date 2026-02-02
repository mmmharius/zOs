ASM     = nasm
CC      = gcc
LD      = ld

ASMFLAGS = -f elf32
CFLAGS   = -m32 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Iinclude
LDFLAGS  = -m elf_i386 -T linker.ld

OBJDIR = obj

all: kernel.bin

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/boot.o: boot/boot.asm | $(OBJDIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJDIR)/kernel.o: src/kernel.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJDIR)/boot.o $(OBJDIR)/kernel.o
	$(LD) $(LDFLAGS) -o $@ $(OBJDIR)/boot.o $(OBJDIR)/kernel.o

iso: kernel.bin
	mv kernel.bin isodir/boot/
	grub-mkrescue -o zOS.iso isodir

run: iso
	qemu-system-i386 -cdrom zOS.iso

clean:
	rm -rf $(OBJDIR)
	rm -f isodir/boot/kernel.bin

fclean: clean
	rm -f zOS.iso

re: fclean all

.PHONY: all iso run clean fclean re
