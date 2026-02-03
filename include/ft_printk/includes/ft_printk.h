#ifndef FT_PRINTK_H
#define FT_PRINTK_H
#include "stdint.h"

#define COM1 0x3F8

extern int ROW;
extern int COL;

#define VGA 0
#define SERIAL 1

int	ft_kputchar(uint8_t c, int output);
int	ft_putstr(char *str, int output);
int ft_putptr(uint32_t addr, int output);
int	ft_putpercent(int output);
int	ft_formats(uint32_t* args, const char format, int output);
int	ft_printk(int output, const char *str, ...);
int	ft_puthex(unsigned int num, char format, int output);
int	ft_putnbr(int n, int output);
int	ft_putnsigned(unsigned int n, int output);

#endif
