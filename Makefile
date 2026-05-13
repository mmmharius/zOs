ASM            = nasm
CC             = cc
LD             = ld
ASMFLAGS       = -f elf32
CFLAGS         = -Wall -Wextra -Werror -m32 -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -Iincludes/ -Ilib/printk_zOs
LDFLAGS        = -m elf_i386 -T linker.ld

OBJ_DIR        = obj
DEBUG_OBJ_DIR  = obj/debug

SRCS           = kernel/kernel.c kernel/keyboard.c kernel/shell/kshell.c \
                 kernel/screen/screen_core.c kernel/screen/screen_display.c \
                 kernel/screen/screen_io.c kernel/screen/screen_utils.c
DEBUG_SRCS     = $(SRCS) kernel/debug.c

OBJS           = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))
DEBUG_OBJS     = $(patsubst %.c,$(DEBUG_OBJ_DIR)/%.o,$(DEBUG_SRCS))

LIB_DIR        = lib
LIBASM_LIB     = $(LIB_DIR)/libasm_zOs/libasm_zOs.a
PRINTK_LIB     = $(LIB_DIR)/printk_zOs/printk_zOs.a
LIBFT_LIB  = $(LIB_DIR)/libft_zOs/libft_zOs.a
LIBS       = $(PRINTK_LIB) $(LIBASM_LIB) $(LIBFT_LIB)

CLEAN_TARGETS  = $(OBJ_DIR) isodir/boot/kernel.bin
FCLEAN_TARGETS = $(OBJ_DIR) isodir/boot/kernel.bin kernel.bin zOs.iso

GREEN  = \033[0;32m
RED    = \033[0;31m
BLUE   = \033[0;34m
RESET  = \033[0m
BOLD   = \033[1m

define run_cmd
	@printf "  $(BLUE)->$(RESET) %-40s" "$(2)"; \
	if $(1) > /tmp/zos_build.log 2>&1; then \
		printf " $(GREEN)[OK]$(RESET)\n"; \
	else \
		printf " $(RED)[KO]$(RESET)\n"; \
		cat /tmp/zos_build.log; \
		exit 1; \
	fi
endef

check_submodules:
	@if [ ! -e lib/printk_zOs/.git ] ||  [ ! -e lib/libasm_zOs/.git ] || [ ! -e lib/libft_zOs/.git ]; then \
		printf "\n$(RED)$(BOLD)Submodules not found in lib/$(RESET)\n"; \
		printf "\n$(BLUE)Initializing submodules...\n$(RESET)"; \
		git submodule update --init --recursive; \
	fi

all: check_submodules banner kernel.bin
	@printf "\n $(GREEN)$(BOLD)Build complete$(RESET) $(GREEN)[OK]$(RESET)\n\n"

banner:
	@printf "\n$(BLUE)$(BOLD)                 zOs build system$(RESET)\n"
	@printf "\n$(BLUE)-----------------------------------------------------\n"



$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/kernel/screen

$(DEBUG_OBJ_DIR):
	@mkdir -p $(DEBUG_OBJ_DIR)/kernel/screen

$(OBJ_DIR)/boot.o: boot/boot.asm | $(OBJ_DIR)
	$(call run_cmd,$(ASM) $(ASMFLAGS) $< -o $@,asm    $<)

$(DEBUG_OBJ_DIR)/boot.o: boot/boot.asm | $(DEBUG_OBJ_DIR)
	$(call run_cmd,$(ASM) $(ASMFLAGS) $< -o $@,asm    $<)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(call run_cmd,$(CC) $(CFLAGS) -c $< -o $@,cc     $<)

$(DEBUG_OBJ_DIR)/%.o: %.c | $(DEBUG_OBJ_DIR)
	@mkdir -p $(dir $@)
	$(call run_cmd,$(CC) $(CFLAGS) -c $< -o $@,cc     $<)

lib_build: FORCE
	@$(MAKE) --no-print-directory -C $(LIB_DIR)

kernel.bin: $(OBJ_DIR)/boot.o $(OBJS) lib_build
	$(call run_cmd,$(LD) $(LDFLAGS) -o $@ $(OBJ_DIR)/boot.o $(OBJS) $(LIBS),link   kernel.bin)

iso: kernel.bin
	@mv kernel.bin isodir/boot/
	$(call run_cmd,grub-mkrescue -o zOs.iso isodir,iso    zOs.iso)

run: iso
	@printf "\n $(BLUE)$(BOLD)Booting zOs in QEMU...$(RESET)\n\n"
	@qemu-system-i386 -cdrom zOs.iso -serial stdio

corr: CFLAGS += -DCORR
corr: all iso
	@printf "\n $(BLUE)$(BOLD)Booting zOs (CORR)...$(RESET)\n\n"
	@qemu-system-i386 -cdrom zOs.iso -serial stdio

debug: CFLAGS += -DDEBUG
debug: $(DEBUG_OBJ_DIR)/boot.o $(DEBUG_OBJS)
	@$(MAKE) --no-print-directory -C $(LIB_DIR) fclean
	@$(MAKE) --no-print-directory -C $(LIB_DIR) EXTRA_CFLAGS="-DDEBUG"
	$(call run_cmd,$(LD) $(LDFLAGS) -o kernel.bin $(DEBUG_OBJ_DIR)/boot.o $(DEBUG_OBJS) $(LIBS),link   kernel.bin)
	@mv kernel.bin isodir/boot/
	$(call run_cmd,grub-mkrescue -o zOs.iso isodir,iso    zOs.iso)
	@printf "\n $(BLUE)$(BOLD)Booting zOs (DEBUG)...$(RESET)\n\n"
	@qemu-system-i386 -cdrom zOs.iso -serial stdio

clean:
	@printf "\n$(RED)$(BOLD)                 zOs clean system$(RESET)\n"
	@printf "\n$(RED)-------------------------------------------------------\n"
	@printf "  $(BLUE)->$(RESET) %-40s $(RED)[DELETED]$(RESET)\n" "$(CLEAN_TARGETS)"
	@$(MAKE) --no-print-directory -C $(LIB_DIR) clean;
	@printf "\n $(GREEN)$(BOLD)repo clean$(RESET) $(GREEN)[OK]$(RESET)\n";
	@rm -rf $(CLEAN_TARGETS)

fclean:
	@printf "\n$(RED)$(BOLD)                 zOs fclean system$(RESET)\n\n"
	@printf "\n$(RED)-------------------------------------------------------\n"
	@rm -rf $(FCLEAN_TARGETS)
	@printf "  $(BLUE)->$(RESET) %-40s $(RED)[DELETED]$(RESET)\n" $(addprefix lib/,$(CLEAN_TARGETS))
	@$(MAKE) --no-print-directory -C $(LIB_DIR) fclean;
	@printf "\n $(GREEN)$(BOLD)repo fclean$(RESET) $(GREEN)[OK]$(RESET)\n";

re: fclean all

FORCE:
.PHONY: all banner clean_banner fclean_banner corr debug iso run clean fclean re lib_build FORCE