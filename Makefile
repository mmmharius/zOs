ASM            = nasm
CC             = gcc
LD             = ld
ASMFLAGS       = -f elf32
CFLAGS         = -Wall -Wextra -Werror -m32 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Iincludes -Iprintk_zOs
LDFLAGS        = -m elf_i386 -T linker.ld

OBJ_DIR        = obj
DEBUG_OBJ_DIR  = obj/debug

SRCS           = kernel/kernel.c kernel/keyboard.c kernel/screen/screen_core.c \
                 kernel/screen/screen_display.c kernel/screen/screen_io.c \
                 kernel/screen/screen_utils.c
DEBUG_SRCS     = $(SRCS) kernel/debug.c

OBJS           = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))
DEBUG_OBJS     = $(patsubst %.c,$(DEBUG_OBJ_DIR)/%.o,$(DEBUG_SRCS))

LIBASM_DIR     = libasm_zOs
LIBASM_LIB     = $(LIBASM_DIR)/libasm_zOs.a
PRINTK_DIR     = printk_zOs
PRINTK_LIB     = $(PRINTK_DIR)/printk_zOs.a
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

define del_path
	@if [ -e "$(1)" ]; then \
		printf "  $(BLUE)->$(RESET) %-40s $(RED)[DELETED]$(RESET)\n" "$(1)"; \
		rm -rf "$(1)"; \
		touch /tmp/zos_cleaned; \
	fi
endef

check_submodules:
	@if [ ! -e printk_zOs/.git ] || [ ! -e libasm_zOs/.git ]; then \
		printf "\n$(RED)$(BOLD)Submodules printk_zOs or libasm_zOs not here$(RESET)\n"; \
		printf "\n$(BLUE)Initializing submodules...\n$(RESET)"; \
		git submodule update --init --recursive; \
	fi

all: check_submodules banner kernel.bin
	@printf "\n $(GREEN)$(BOLD)Build complete$(RESET) $(GREEN)[OK]$(RESET)\n\n"

banner:
	@printf "\n$(BLUE)$(BOLD)                 zOs build system$(RESET)\n"
	@printf "\n$(BLUE)-----------------------------------------------------\n"

clean_banner:
	@printf "\n$(BLUE)$(BOLD)                 zOs clean system$(RESET)\n"
	@printf "\n$(BLUE)-----------------------------------------------------\n"

fclean_banner:
	@printf "\n$(RED)$(BOLD)                 zOs fclean system$(RESET)\n\n"
	@printf "\n$(RED)-----------------------------------------------------\n"


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

$(LIBASM_LIB): FORCE
	@$(MAKE) -s -C $(LIBASM_DIR)

$(PRINTK_LIB): FORCE
	@$(MAKE) -s -C $(PRINTK_DIR)

kernel.bin: $(OBJ_DIR)/boot.o $(OBJS) $(PRINTK_LIB) $(LIBASM_LIB)
	$(call run_cmd,$(LD) $(LDFLAGS) -o $@ $(OBJ_DIR)/boot.o $(OBJS) $(PRINTK_LIB) $(LIBASM_LIB),link   kernel.bin)

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
debug: $(DEBUG_OBJ_DIR)/boot.o $(DEBUG_OBJS) $(LIBASM_LIB)
	@$(MAKE) -s -C $(PRINTK_DIR) fclean
	@$(MAKE) -s -C $(PRINTK_DIR) EXTRA_CFLAGS=-DDEBUG
	$(call run_cmd,$(LD) $(LDFLAGS) -o kernel.bin $(DEBUG_OBJ_DIR)/boot.o $(DEBUG_OBJS) $(PRINTK_LIB) $(LIBASM_LIB),link   kernel.bin)
	@mv kernel.bin isodir/boot/
	$(call run_cmd,grub-mkrescue -o zOs.iso isodir,iso    zOs.iso)
	@printf "\n $(BLUE)$(BOLD)Booting zOs (DEBUG)...$(RESET)\n\n"
	@qemu-system-i386 -cdrom zOs.iso -serial stdio

clean: clean_banner
	@cleaned=0; \
	for f in $(CLEAN_TARGETS); do \
		if [ -e "$$f" ]; then \
			printf "  $(BLUE)->$(RESET) %-40s $(RED)[DELETED]$(RESET)\n" "$$f"; \
			rm -rf "$$f"; \
			cleaned=1; \
		fi; \
	done; \
	$(MAKE) -s -C $(LIBASM_DIR) clean; \
	$(MAKE) -s -C $(PRINTK_DIR) clean; \
	if [ $$cleaned -eq 1 ]; then \
		printf "\n $(GREEN)$(BOLD)repo clean$(RESET) $(GREEN)[OK]$(RESET)\n"; \
	else \
		printf "\n $(BLUE)$(BOLD)already clean$(RESET)\n"; \
	fi

fclean: fclean_banner
	@cleaned=0; \
	for f in $(FCLEAN_TARGETS); do \
		if [ -e "$$f" ]; then \
			printf "  $(BLUE)->$(RESET) %-40s $(RED)[DELETED]$(RESET)\n" "$$f"; \
			rm -rf "$$f"; \
			cleaned=1; \
		fi; \
	done; \
	$(MAKE) -s -C $(LIBASM_DIR) fclean; \
	$(MAKE) -s -C $(PRINTK_DIR) fclean; \
	if [ $$cleaned -eq 1 ]; then \
		printf "\n $(GREEN)$(BOLD)repo fclean$(RESET) $(GREEN)[OK]$(RESET)\n"; \
	else \
		printf "\n $(BLUE)$(BOLD)already clean$(RESET)\n"; \
	fi

re: fclean all

FORCE:
.PHONY: all banner clean_banner fclean_banner corr debug iso run clean fclean re FORCE