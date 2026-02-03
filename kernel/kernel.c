#include "kernel.h"
#include "keyboard.h"
#include "screen.h"

int COL = 0;
int ROW = 0;

void    check_col() {
    if (COL >= 80) {
        COL = 0;
        ROW++;
    }
    // if (ROW >= 25) {
    //     //scrol ???
    // }
}

void    main() {
    print_42();
    keyboard_loop();
}
