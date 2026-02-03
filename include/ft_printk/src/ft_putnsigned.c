#include "ft_printk.h"

int	ft_putnsigned(unsigned int n, int output) {
	int nb_caracter = 0;
	
	if (n > 9)
		nb_caracter += ft_putnsigned(n / 10, output);
	nb_caracter += ft_kputchar((n % 10) + '0', output);
	return nb_caracter;
}