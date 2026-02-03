#ifndef FT_KPRINTF_H
# define FT_KPRINTF_H
#include "stdint.h"

extern int ROW;
extern int COL;

int	ft_kputchar(int c);
int	ft_putstr(char *str);
int ft_putptr(uint32_t addr);
int	ft_putpercent(void);
int	ft_formats(uint32_t* args, const char format);
int	ft_printf(const char *str, ...);
int	ft_puthex(unsigned int num, char format);
int	ft_putnbr(int n);
int	ft_putnsigned(unsigned int n);
int	ft_print_pointer(unsigned long long addr, int fd);

#endif
