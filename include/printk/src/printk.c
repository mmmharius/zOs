#include "printk.h"
#include "../../screen.h"
#include "../../io.h"

int	ft_kputchar(uint8_t c, int output) {
	if (output == SERIAL) {
		while ((inb(COM1 + 5) & 0x20) == 0);
    		outb(COM1, c);
		return 1;
	}
	volatile uint16_t* vga = (uint16_t*)0xB8000;
	
	if (c == '\n') {
		COL = 0;
		ROW++;
		check_col();
	}
	else {
		vga[ROW * 80 + COL] = (uint16_t)c | 0x0F00;
		COL++;
		check_col();
	}
	return 1;
}

/*
	serial port com1 address : 0x3F8
	0x3F8 + 0 = Data Register
	0x3F8 + 1 = Interrupt Enable Register
	0x3F8 + 2 = Interrupt Identification Register
	0x3F8 + 3 = Line Control Register
	0x3F8 + 4 = Modem Control Register
	0x3F8 + 5 = Line Status Register
	0x3F8 + 6 = Modem Status Register
	0x3F8 + 7 = Scratch Register

	inb(0x3F8) -> 0x20 = true

*/

int	ft_putstr(char *str, int output) {
	int	j;

	j = 0;
	if (!str)
		str = "(null)";
	while (str[j])
	{
		ft_kputchar(str[j], output);
		j++;
	}
	return (j);
}

int	ft_putpercent(int output) {
	return (ft_kputchar('%', output));
}

int	ft_formats(uint32_t* args, const char format, int output) {
	int	nb_caracter = 0;
	uint32_t val = *args;

	if (format == 'c')
		nb_caracter += ft_kputchar((char)val, output);
	else if (format == 's')
		nb_caracter += ft_putstr((char*)val, output);
	else if (format == 'p')
		nb_caracter += ft_putptr(val, output);
	else if (format == 'd' || format == 'i')
		nb_caracter += ft_putnbr((int)val, output);
	else if (format == 'u')
		nb_caracter += ft_putnsigned(val, output);
	else if (format == 'x' || format == 'X')
		nb_caracter += ft_puthex(val, format, output);
	else if (format == '%')
		nb_caracter += ft_putpercent(output);
	return nb_caracter;
}

int	printk(int output, const char *str, ...) {
	uint32_t* args = (uint32_t*)(&str + 1);
	int nb_caracter = 0;

	for (int i = 0; str[i]; i++) {
		if (str[i] == '%') {
			nb_caracter += ft_formats(args, str[i + 1], output);
			args++;
			i++;
		}
		else
			nb_caracter += ft_kputchar(str[i], output);
	}
	return nb_caracter;
}
