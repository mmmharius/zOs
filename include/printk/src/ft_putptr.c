#include "printk.h"

int ft_putptr(uint32_t addr, int output) {
    int nb_caracter = 0;
    nb_caracter += ft_putstr("0x", output);
    
    unsigned int high = (addr >> 16) & 0xFFFF;
    unsigned int low = addr & 0xFFFF;
    
    if (high)
        nb_caracter += ft_puthex(high, 'x', output);
    nb_caracter += ft_puthex(low, 'x', output);
    
    return nb_caracter;
}
