
#include "ft_kprintf.h"

int	ft_putnsigned(unsigned int n) {
	int nb_caracter = 0;
	
	if (n > 9)
		nb_caracter += ft_putnsigned(n / 10);
	nb_caracter += ft_kputchar((n % 10) + '0');
	return nb_caracter;
}