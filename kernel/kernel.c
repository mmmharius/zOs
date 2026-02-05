#include <keyboard.h>
#include <screen.h>
#include <printk.h>
#ifdef DEBUG
    #include "debug.h"
#endif

int COL = 0;
int ROW = 0;
int SCREEN = 1;

t_screen screens[NB_SCREEN];
t_screen *current;

t_screen s_creen = {
    .row = 0,
    .col = 0,
    .scroll = 0
};

int main() {
    init_screen();
    
#ifdef DEBUG
    printk(0, "DEBUG VERSION ZZZZZ   OOO   SSSS  DEBUG VERSION\n");
    printk(0, "DEBUG VERSION    Z   O   O  S     DEBUG VERSION\n");
    printk(0, "DEBUG VERSION   Z    O   O   SSS  DEBUG VERSION\n");
    printk(0, "DEBUG VERSION  Z     O   O      S DEBUG VERSION\n");
    printk(0, "DEBUG VERSION ZZZZZ   OOO   SSSSS DEBUG VERSION\n");
    print_screen_struct();
    print_current_struct();
#else
    printk(0, "ZZZZZ   OOO   SSSS\n");
    printk(0, "   Z   O   O  S    \n");
    printk(0, "  Z    O   O   SSS \n");
    printk(0, " Z     O   O      S\n");
    printk(0, "ZZZZZ   OOO   SSSSS\n");
#endif
    
    move_cursor();
    keyboard_loop();
    while(1);
    return 1;
}