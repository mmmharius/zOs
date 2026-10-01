#include "kernel.h"

int main(void) {
    gdt_init();
    screen_init();
    kshell_run();
    return 0;
}