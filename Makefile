ASM     = nasm
CC      = gcc
LD      = ld

ASMFLAGS = -f elf32
CFLAGS   = -m32 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Iinclude
LDFLAGS  = -m elf_i386 -T linker.ld

OBJ_DIR = obj
LIBASM_DIR = kernel/libasm
LIBASM = $(LIBASM_DIR)/libasm.a

all: kernel.bin

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/boot.o: boot/boot.asm | $(OBJ_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/kernel.o: kernel/kernel.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBASM):
	$(MAKE) -C $(LIBASM_DIR)

kernel.bin: $(OBJ_DIR)/boot.o $(OBJ_DIR)/kernel.o $(LIBASM)
	$(LD) $(LDFLAGS) -o $@ $(OBJ_DIR)/boot.o $(OBJ_DIR)/kernel.o $(LIBASM)

iso: kernel.bin
	mv kernel.bin isodir/boot/
	grub-mkrescue -o zOS.iso isodir

run: iso
	qemu-system-i386 -cdrom zOS.iso

clean:
	rm -rf $(OBJ_DIR)
	rm -f isodir/boot/kernel.bin
	$(MAKE) -C $(LIBASM_DIR) clean

fclean: clean
	rm -f zOS.iso
	$(MAKE) -C $(LIBASM_DIR) fclean

re: fclean all

.PHONY: all iso run clean fclean re
