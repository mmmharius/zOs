NAME = ft_kprintf.a
CC = gcc
CFLAGS = -m32 -Wall -Wextra -Werror -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs
SRC = src/ft_kprintf.c src/ft_puthex.c src/ft_putnbr.c src/ft_putnsigned.c src/ft_putptr.c
OBJ_DIR = obj
OBJ = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRC))
INCLUDE = -Iincludes

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $@ $(OBJ)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re