
#include "ft_kprintf.h"

int	ft_putnbr(int n) {
	int	nb_caracter;

	nb_caracter = 0;
	if (n == -2147483648)
		return (ft_putstr("-2147483648"));
	if (n < 0)
	{
		nb_caracter += ft_kputchar('-');
		n = -n;
	}
	if (n > 9)
		nb_caracter += ft_putnbr(n / 10);
	nb_caracter += ft_kputchar((n % 10) + '0');
	return (nb_caracter);
}
