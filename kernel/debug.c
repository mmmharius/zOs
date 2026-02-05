#include <debug.h>

void    print_screen_struct() {
    for (int i = 0; i < NB_SCREEN; i++) {
        printk(1, "\n\nSCREEN : %d\n\n", i);
        for (int row = 0; row < (VGA_HEIGHT * SCREEN_LENGHT); row++) {
            for (int col = 0; col < VGA_WIDTH; col++) {
                printk(1, "%c", screens[i].buffer[row * VGA_WIDTH + col]);
            }
            printk(1, "\n");
        }
    }
}

void    print_current_struct() {
    int id = current - screens;
    printk(1, "SCREEN : %d\n\n", id);
    for (int row = 0; row < (VGA_HEIGHT * SCREEN_LENGHT); row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            printk(1, "%c", screens[id].buffer[row * VGA_WIDTH + col]);
        }
        printk(1, "\n");
    }
}