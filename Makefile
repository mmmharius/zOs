ASM     = nasm
CC      = gcc
LD      = ld

ASMFLAGS = -f elf32
CFLAGS   = -Wall -Wextra -Werror -m32 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Iinclude -Iinclude/printk/includes
LDFLAGS  = -m elf_i386 -T linker.ld

OBJ_DIR = obj
LIBASM_DIR = include/libasm
LIBASM = $(LIBASM_DIR)/libasm.a

SRCS = kernel/kernel.c kernel/keyboard.c kernel/screen.c
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

PRINTK_DIR = include/printk
PRINTK_LIB = $(PRINTK_DIR)/printk.a

all: kernel.bin

$(PRINTK_LIB): FORCE
	$(MAKE) -C $(PRINTK_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/kernel

$(OBJ_DIR)/boot.o: boot/boot.asm | $(OBJ_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBASM): FORCE
	$(MAKE) -C $(LIBASM_DIR)

kernel.bin: $(OBJ_DIR)/boot.o $(OBJS) $(PRINTK_LIB) $(LIBASM)
	$(LD) $(LDFLAGS) -o $@ $(OBJ_DIR)/boot.o $(OBJS) $(PRINTK_LIB) $(LIBASM)

iso: kernel.bin
	mv kernel.bin isodir/boot/
	grub-mkrescue -o zOS.iso isodir

run: iso
	qemu-system-i386 -cdrom zOS.iso -serial stdio

clean:
	rm -rf $(OBJ_DIR)
	rm -f isodir/boot/kernel.bin
	$(MAKE) -C $(LIBASM_DIR) clean
	$(MAKE) -C $(PRINTK_DIR) clean

fclean: clean
	rm -f kernel.bin zOS.iso
	$(MAKE) -C $(LIBASM_DIR) fclean
	$(MAKE) -C $(PRINTK_DIR) fclean

re: fclean all

FORCE:

.PHONY: all iso run clean fclean re FORCE