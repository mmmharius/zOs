#include "kernel.h"
#include "keyboard.h"
#include "screen.h"
#include "ft_printk.h"

int COL = 0;
int ROW = 0;

int    main() {
    // print_42();
    int c = 12;
    ft_printk(1, "Hello from ft_kprintf!\nHello from ft_kprintf!\nHello from ft_kprintf!\nHello from ft_kprintf!\n");
    ft_printk(0, "Teste : %d", c);
    move_cursor();
    keyboard_loop();
    while(1);
    return 1;
}
