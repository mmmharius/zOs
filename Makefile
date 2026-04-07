ASM      		= nasm
CC       		= gcc
LD       		= ld
ASMFLAGS 		= -f elf32
CFLAGS   		= -Wall -Wextra -Werror -m32 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Iincludes -Iprintk_zOs
LDFLAGS  		= -m elf_i386 -T linker.ld

OBJ_DIR       	= obj
DEBUG_OBJ_DIR 	= obj/debug
OBJS       		= $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))
DEBUG_OBJS 		= $(patsubst %.c,$(DEBUG_OBJ_DIR)/%.o,$(DEBUG_SRCS))
DEBUG_SRCS 		= $(SRCS) kernel/debug.c
LIBASM_DIR 		= libasm_zOs
LIBASM     		= $(LIBASM_DIR)/libasm_zOs.a
PRINTK_DIR 		= printk_zOs
PRINTK_LIB 		= $(PRINTK_DIR)/printk_zOs.a

SRCS 			= kernel/kernel.c kernel/keyboard.c kernel/screen/screen_core.c \
	 			  kernel/screen/screen_display.c kernel/screen/screen_io.c \
	 			  kernel/screen/screen_utils.c


all: kernel.bin

corr: CFLAGS += -DCORR
corr: $(OBJ_DIR)/boot.o $(OBJS) $(PRINTK_LIB) $(LIBASM)
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJ_DIR)/boot.o $(OBJS) $(PRINTK_LIB) $(LIBASM)
	mv kernel.bin isodir/boot/
	grub-mkrescue -o zOs.iso isodir
	qemu-system-i386 -cdrom zOs.iso -serial stdio

debug: CFLAGS += -DDEBUG
debug: $(DEBUG_OBJ_DIR)/boot.o $(DEBUG_OBJS) $(LIBASM)
	$(MAKE) -C $(PRINTK_DIR) fclean
	$(MAKE) -C $(PRINTK_DIR) EXTRA_CFLAGS=-DDEBUG
	$(LD) $(LDFLAGS) -o kernel.bin $(DEBUG_OBJ_DIR)/boot.o $(DEBUG_OBJS) $(PRINTK_LIB) $(LIBASM)
	mv kernel.bin isodir/boot/
	grub-mkrescue -o zOs.iso isodir
	qemu-system-i386 -cdrom zOs.iso -serial stdio

$(PRINTK_LIB): FORCE
	$(MAKE) -C $(PRINTK_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/kernel/screen

$(DEBUG_OBJ_DIR):
	mkdir -p $(DEBUG_OBJ_DIR)/kernel/screen

$(OBJ_DIR)/boot.o: boot/boot.asm | $(OBJ_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(DEBUG_OBJ_DIR)/boot.o: boot/boot.asm | $(DEBUG_OBJ_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEBUG_OBJ_DIR)/%.o: %.c | $(DEBUG_OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBASM): FORCE
	$(MAKE) -C $(LIBASM_DIR)

kernel.bin: $(OBJ_DIR)/boot.o $(OBJS) $(PRINTK_LIB) $(LIBASM)
	$(LD) $(LDFLAGS) -o $@ $(OBJ_DIR)/boot.o $(OBJS) $(PRINTK_LIB) $(LIBASM)

iso: kernel.bin
	mv kernel.bin isodir/boot/
	grub-mkrescue -o zOs.iso isodir

run: iso
	qemu-system-i386 -cdrom zOs.iso -serial stdio

clean:
	rm -rf $(OBJ_DIR)
	rm -f isodir/boot/kernel.bin
	$(MAKE) -C $(LIBASM_DIR) clean
	$(MAKE) -C $(PRINTK_DIR) clean

fclean: clean
	rm -f kernel.bin zOs.iso
	$(MAKE) -C $(LIBASM_DIR) fclean
	$(MAKE) -C $(PRINTK_DIR) fclean

re: fclean all

FORCE:

.PHONY: all corr debug iso run clean fclean re FORCE
