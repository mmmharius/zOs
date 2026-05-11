#include "kernel.h"

void zos(void) {
    #ifdef CORR
        printk(0, "42\n");
    #elif defined(DEBUG)
        printk(0, "ZOS : DEBUG MODE ON\n");
    #else
        printk(0, "ZZZZZ   OOO   SSSS \n");
        printk(0, "   Z   O   O  S    \n");
        printk(0, "  Z    O   O   SSS \n");
        printk(0, " Z     O   O      S\n");
        printk(0, "ZZZZZ   OOO   SSSSS\n");
    #endif
}

int main(void) {
    screen_init();
    zos();
    kshell_run();
    return 0;
}