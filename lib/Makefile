ASM      = nasm
CC       = gcc
ASMFLAGS = -f elf32
CFLAGS   = -m32 -Wall -Wextra -Werror -fno-builtin -fno-stack-protector \
           -nostdlib -nodefaultlibs -I../includes $(EXTRA_CFLAGS)

LIBASM_SRCS = libasm_zOs/ft_strlen.s libasm_zOs/ft_strcpy.s libasm_zOs/ft_strcmp.s
LIBASM_OBJS = $(patsubst libasm_zOs/%.s,libasm_zOs/obj/%.o,$(LIBASM_SRCS))
LIBASM      = libasm_zOs/libasm_zOs.a

PRINTK_SRCS = printk_zOs/src/printk.c     printk_zOs/src/ft_puthex.c \
              printk_zOs/src/ft_putnbr.c  printk_zOs/src/ft_putnsigned.c \
              printk_zOs/src/ft_putptr.c
PRINTK_OBJS = $(patsubst printk_zOs/src/%.c,printk_zOs/obj/%.o,$(PRINTK_SRCS))
PRINTK      = printk_zOs/printk_zOs.a

LIBC_SRCS  = libc_zOs/ft_memset.c   libc_zOs/ft_bzero.c    \
              libc_zOs/ft_memcpy.c   libc_zOs/ft_memmove.c  \
              libc_zOs/ft_atoi.c     libc_zOs/ft_strchr.c   \
              libc_zOs/ft_strrchr.c  libc_zOs/ft_strncmp.c  \
              libc_zOs/ft_strlcpy.c  libc_zOs/ft_strlcat.c  \
              libc_zOs/ft_isalpha.c  libc_zOs/ft_isdigit.c  \
              libc_zOs/ft_isalnum.c  libc_zOs/ft_toupper.c  \
              libc_zOs/ft_tolower.c
LIBC_OBJS  = $(patsubst libc_zOs/%.c,libc_zOs/obj/%.o,$(LIBC_SRCS))
LIBC       = libc_zOs/libc_zOs.a

GREEN = \033[0;32m
RED   = \033[0;31m
BLUE  = \033[0;34m
RESET = \033[0m

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

all: $(LIBASM) $(PRINTK) $(LIBC)

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

libc_zOs/obj:
	@mkdir -p $@

libc_zOs/obj/%.o: libc_zOs/%.c | libc_zOs/obj
	$(call run_cmd,$(CC) $(CFLAGS) -c $< -o $@,cc     $<)

$(LIBC): $(LIBC_OBJS)
	@ar rcs $@ $(LIBC_OBJS)

clean:
	@printf "  $(BLUE)->$(RESET) %-40s $(RED)[DELETED]$(RESET)\n" \
		"libasm_zOs/obj printk_zOs/obj libc_zOs/obj"
	@rm -rf libasm_zOs/obj printk_zOs/obj libc_zOs/obj

fclean: clean
	@printf "  $(BLUE)->$(RESET) %-40s $(RED)[DELETED]$(RESET)\n" \
		"$(LIBASM) $(PRINTK) $(LIBC)"
	@rm -f $(LIBASM) $(PRINTK) $(LIBC)

re: fclean all

.PHONY: all clean fclean re