ASM     = nasm
CC      = gcc
LD      = ld

ASMFLAGS = -f elf32
CFLAGS   = -m32 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Iinclude -Iinclude/ft_kprintf/includes
LDFLAGS  = -m elf_i386 -T linker.ld

OBJ_DIR = obj
LIBASM_DIR = include/libasm
LIBASM = $(LIBASM_DIR)/libasm.a

SRCS = kernel/kernel.c kernel/keyboard.c kernel/screen.c
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

FT_KPRINTF_DIR = include/ft_kprintf
FT_KPRINTF_LIB = $(FT_KPRINTF_DIR)/ft_kprintf.a

all: kernel.bin

$(FT_KPRINTF_LIB): FORCE
	$(MAKE) -C $(FT_KPRINTF_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/kernel

$(OBJ_DIR)/boot.o: boot/boot.asm | $(OBJ_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBASM): FORCE
	$(MAKE) -C $(LIBASM_DIR)

kernel.bin: $(OBJ_DIR)/boot.o $(OBJS) $(FT_KPRINTF_LIB) $(LIBASM)
	$(LD) $(LDFLAGS) -o $@ $(OBJ_DIR)/boot.o $(OBJS) $(FT_KPRINTF_LIB) $(LIBASM)

iso: kernel.bin
	mv kernel.bin isodir/boot/
	grub-mkrescue -o zOS.iso isodir

run: iso
	qemu-system-i386 -cdrom zOS.iso

clean:
	rm -rf $(OBJ_DIR)
	rm -f isodir/boot/kernel.bin
	$(MAKE) -C $(LIBASM_DIR) clean
	$(MAKE) -C $(FT_KPRINTF_DIR) clean

fclean: clean
	rm -f kernel.bin zOS.iso
	$(MAKE) -C $(LIBASM_DIR) fclean
	$(MAKE) -C $(FT_KPRINTF_DIR) fclean

re: fclean all

FORCE:

.PHONY: all iso run clean fclean re FORCE