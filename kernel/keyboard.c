#include <keyboard.h>
#include <screen.h>
#include <stdint.h>
#include <io.h>
#include <printk.h>
#include <color.h>
#ifdef DEBUG
    #include "debug.h"
#endif

unsigned char read_keyboard() {
    while ((inb(KB_STATUS) & 1) == 0);
    return inb(KB_DATA);
}

char    scancode_to_ascii(unsigned char sc) {
    switch(sc) {
        case KEY_Q: return 'Q';
        case KEY_W: return 'W';
        case KEY_E: return 'E';
        case KEY_R: return 'R';
        case KEY_T: return 'T';
        case KEY_Y: return 'Y';
        case KEY_U: return 'U';
        case KEY_I: return 'I';
        case KEY_O: return 'O';
        case KEY_P: return 'P';
        case KEY_A: return 'A';
        case KEY_S: return 'S';
        case KEY_D: return 'D';
        case KEY_F: return 'F';
        case KEY_G: return 'G';
        case KEY_H: return 'H';
        case KEY_J: return 'J';
        case KEY_K: return 'K';
        case KEY_L: return 'L';
        case KEY_Z: return 'Z';
        case KEY_X: return 'X';
        case KEY_C: return 'C';
        case KEY_V: return 'V';
        case KEY_B: return 'B';
        case KEY_N: return 'N';
        case KEY_M: return 'M';
        case KEY_ENTER: return '\n';
        case KEY_BACKSPACE: return '\b';
        case KEY_SHIFT: return '1';
        case KEY_SPACE: return ' ';
        default: return '?';
    }
}

void handle_backspace() {
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;

    if (current->col > 0) {
        current->col--;
        vga[current->row * VGA_WIDTH + current->col] = ' ' | VGA_DEFAULT_COLOR;
    } 
    else if (current->row > START_PRINT) {
        current->row--;
        current->col = 0;
        for (int col = 79; col >= 0; col--) {
            char ch = vga[current->row * VGA_WIDTH + col] & 0x00FF;
            if (ch != ' ' && ch != 0) {
                current->col = col + 1;
                break;
            }
        }
    }
    move_cursor();
}

void    print_keyboard(char c) {
    if (c == '\n') {
        current->col = 0;
        current->row++;
        check_col();
        move_cursor();
    }
    else if (c == '\b') {
        handle_backspace();
    }
    else
        print_char(c);
    move_cursor();
    #ifdef DEBUG
        int id = current - screens;
        printk(1, "STRUCT\nscreen:%d\nstart_print:%d\nrow:%d\ncol:%d\nscroll:%d\n\n\n", id, START_PRINT, current->row, current->col, current->scroll);
    #endif
}

void    keyboard_loop() {
    while(1) {
        unsigned char sc = read_keyboard();
        unsigned char key = sc & 0x7F;
        if (sc & KEY_RELEASE) {
            continue;
        }
        else if (key == KEY_TAB) {
            switch_screen((current - screens + 1) % NB_SCREEN);
            continue;
        }
        else {
            char c = scancode_to_ascii(sc);
            print_keyboard(c);
        }
    }
}