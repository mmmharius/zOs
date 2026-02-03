#include "kernel.h"
#include "keyboard.h"
#include "screen.h"
#include "printk.h"

int COL = 0;
int ROW = 0;

int    main() {
    // print_42();
    int c = 12;
    printk(1, "Hello from kprintf!\nHello from kprintf!\nHello from kprintf!\nHello from kprintf!\n");
    printk(0, "Teste : %d", c);
    move_cursor();
    keyboard_loop();
    while(1);
    return 1;
}
