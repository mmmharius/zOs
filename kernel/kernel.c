#include <keyboard.h>
#include <screen.h>
#include <printk.h>

int COL = 0;
int ROW = 0;
int SCREEN = 1;

t_screen screens[NB_SCREEN - 1];
t_screen *current;

t_screen s_creen = {
    .row = 0,
    .col = 0,
    .scroll = 0
};

int main() {
    init_screen();
    printk(1, "Hello from kprintf!\nHello from kprintf!\nHello from kprintf!\nHello from kprintf!\n");
    printk(0, "ZZZZZ   OOO   SSSS\n");
    printk(0, "   Z   O   O  S    \n");
    printk(0, "  Z    O   O   SSS \n");
    printk(0, " Z     O   O      S\n");
    printk(0, "ZZZZZ   OOO   SSSSS\n");
    move_cursor();
    keyboard_loop();
    while(1);
    return 1;
}