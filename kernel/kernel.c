#include "kernel.h"

int main(void) {
    screen_init();
    kshell_run();
    return 0;
}