
#include "ft_kprintf.h"

int	ft_putptr(uint32_t addr) {
	ft_putstr("0x");
	unsigned int high = (addr >> 16) & 0xFFFF;
	unsigned int low = addr & 0xFFFF;
	if (high)
		ft_puthex(high, 'x');
	ft_puthex(low, 'x');
	return 0;
}
