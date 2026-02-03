#include "printk.h"

int	ft_putnbr(int n, int output) {
	int	nb_caracter = 0;

	nb_caracter = 0;
	if (n == -2147483648)
		return (ft_putstr("-2147483648", output));
	if (n < 0)
	{
		nb_caracter += ft_kputchar('-', output);
		n = -n;
	}
	if (n > 9)
		nb_caracter += ft_putnbr(n / 10, output);
	nb_caracter += ft_kputchar((n % 10) + '0', output);
	return (nb_caracter);
}
