#include "ft_printk.h"

int ft_puthex(unsigned int num, char format, int output) {
    int nb_caracter = 0;
    char *base;
    
    if (format == 'x')
        base = "0123456789abcdef";
    else
        base = "0123456789ABCDEF";
    if (num >= 16)
        nb_caracter += ft_puthex(num / 16, format, output);
    nb_caracter += ft_kputchar(base[num % 16], output);
    return (nb_caracter);
}
