ASM     = nasm
CC      = gcc
LD      = ld

ASMFLAGS = -f elf32
CFLAGS   = -Wall -Wextra -Werror -m32 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Iinclude -Iinclude/ft_printk/includes
LDFLAGS  = -m elf_i386 -T linker.ld

OBJ_DIR = obj
LIBASM_DIR = include/libasm
LIBASM = $(LIBASM_DIR)/libasm.a

SRCS = kernel/kernel.c kernel/keyboard.c kernel/screen.c
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

FT_PRINTK_DIR = include/ft_printk
FT_PRINTK_LIB = $(FT_PRINTK_DIR)/ft_printk.a

all: kernel.bin

$(FT_PRINTK_LIB): FORCE
	$(MAKE) -C $(FT_PRINTK_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/kernel

$(OBJ_DIR)/boot.o: boot/boot.asm | $(OBJ_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBASM): FORCE
	$(MAKE) -C $(LIBASM_DIR)

kernel.bin: $(OBJ_DIR)/boot.o $(OBJS) $(FT_PRINTK_LIB) $(LIBASM)
	$(LD) $(LDFLAGS) -o $@ $(OBJ_DIR)/boot.o $(OBJS) $(FT_PRINTK_LIB) $(LIBASM)

iso: kernel.bin
	mv kernel.bin isodir/boot/
	grub-mkrescue -o zOS.iso isodir

run: iso
	qemu-system-i386 -cdrom zOS.iso -serial stdio

clean:
	rm -rf $(OBJ_DIR)
	rm -f isodir/boot/kernel.bin
	$(MAKE) -C $(LIBASM_DIR) clean
	$(MAKE) -C $(FT_PRINTK_DIR) clean

fclean: clean
	rm -f kernel.bin zOS.iso
	$(MAKE) -C $(LIBASM_DIR) fclean
	$(MAKE) -C $(FT_PRINTK_DIR) fclean

re: fclean all

FORCE:

.PHONY: all iso run clean fclean re FORCE