ASM      = nasm
CC       = gcc
ASMFLAGS = -f elf32
CFLAGS   = -m32 -Wall -Wextra -Werror -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -I../includes

LIBASM_SRCS = libasm_zOs/ft_strlen.s libasm_zOs/ft_strcpy.s libasm_zOs/ft_strcmp.s
LIBASM_OBJS = $(patsubst libasm_zOs/%.s,libasm_zOs/obj/%.o,$(LIBASM_SRCS))
LIBASM      = libasm_zOs/libasm_zOs.a

PRINTK_SRCS = printk_zOs/src/printk.c printk_zOs/src/ft_puthex.c \
              printk_zOs/src/ft_putnbr.c printk_zOs/src/ft_putnsigned.c \
              printk_zOs/src/ft_putptr.c
PRINTK_OBJS = $(patsubst printk_zOs/src/%.c,printk_zOs/obj/%.o,$(PRINTK_SRCS))
PRINTK      = printk_zOs/printk_zOs.a

CLEAN_TARGETS  = libasm_zOs/obj printk_zOs/obj
FCLEAN_TARGETS = $(CLEAN_TARGETS) $(LIBASM) $(PRINTK)

GREEN  = \033[0;32m
RED    = \033[0;31m
BLUE   = \033[0;34m
RESET  = \033[0m

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

all: $(LIBASM) $(PRINTK)

libasm_zOs/obj:
	@mkdir -p $@

libasm_zOs/obj/%.o: libasm_zOs/%.s | libasm_zOs/obj
	$(call run_cmd,$(ASM) $(ASMFLAGS) $< -o $@,asm    $<)

$(LIBASM): $(LIBASM_OBJS)
	@ar rcs $@ $(LIBASM_OBJS)

printk_zOs/obj:
	@mkdir -p $@

printk_zOs/obj/%.o: printk_zOs/src/%.c | printk_zOs/obj
	$(call run_cmd,$(CC) $(CFLAGS) -c $< -o $@,cc     $<)

$(PRINTK): $(PRINTK_OBJS)
	@ar rcs $@ $(PRINTK_OBJS)

clean:
	@printf "  $(BLUE)->$(RESET) %-40s $(RED)[DELETED]$(RESET)\n" $(addprefix lib/,$(CLEAN_TARGETS))
	@rm -rf $(CLEAN_TARGETS)

fclean:
	@printf "  $(BLUE)->$(RESET) %-40s $(RED)[DELETED]$(RESET)\n" $(addprefix lib/,$(FCLEAN_TARGETS))
	@rm -rf $(FCLEAN_TARGETS)

re: fclean all

.PHONY: all clean fclean re
