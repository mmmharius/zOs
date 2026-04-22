#include "kernel.h"

void    zos() {
    #ifdef CORR
        printk(0, "42\n");
    #elif defined(DEBUG)
        printk(0, "   DEBUG VERSION   \n");
        // print_all_screen();
    #else
        printk(0, "ZZZZZ   OOO   SSSS \n");
        printk(0, "   Z   O   O  S    \n");
        printk(0, "  Z    O   O   SSS \n");
        printk(0, " Z     O   O      S\n");
        printk(0, "ZZZZZ   OOO   SSSSS\n");
    #endif
}

int main() {
    screen_init();
    zos();
    kshell_run();
    while(1);
    return 1;
}
