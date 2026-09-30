#include "kernel.h"

int main(void) {
    gdt_set_entry(0, 0x12345678, 0x6767, 0xCD, 0xBB);
    screen_init();
    kshell_run();
    return 0;
}