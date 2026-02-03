#include "ft_kprintf.h"

int	ft_kputchar(int c) {
	volatile uint16_t* vga = (uint16_t*)0xB8000;
	
	vga[ROW * 80 + COL] = (uint16_t)c | 0x0F00;
	COL++;
	if (COL >= 80) {
		COL = 0; ROW++;
	}
	return 1;
}

int	ft_putstr(char *str) {
	int	j;

	j = 0;
	if (!str)
		str = "(null)";
	while (str[j])
	{
		ft_kputchar(str[j]);
		j++;
	}
	return (j);
}

int	ft_putpercent(void) {
	return (ft_kputchar('%'));
}

int	ft_formats(uint32_t* args, const char format) {
	int	nb_caracter = 0;
	uint32_t val = *args;

	if (format == 'c')
		nb_caracter += ft_kputchar((char)val);
	else if (format == 's')
		nb_caracter += ft_putstr((char*)val);
	else if (format == 'p')
		nb_caracter += ft_putptr(val);
	else if (format == 'd' || format == 'i')
		nb_caracter += ft_putnbr((int)val);
	else if (format == 'u')
		nb_caracter += ft_putnsigned(val);
	else if (format == 'x' || format == 'X')
		nb_caracter += ft_puthex(val, format);
	else if (format == '%')
		nb_caracter += ft_putpercent();
	return nb_caracter;
}

int	ft_kprintf(const char *str, ...) {
	uint32_t* args = (uint32_t*)(&str + 1);
	int nb_caracter = 0;

	for (int i = 0; str[i]; i++) {
		if (str[i] == '%') {
			nb_caracter += ft_formats(args, str[i + 1]);
			args++;
			i++;
		}
		else
			nb_caracter += ft_kputchar(str[i]);
	}
	return nb_caracter;
}
