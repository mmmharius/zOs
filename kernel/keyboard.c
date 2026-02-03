#include "keyboard.h"
#include "screen.h"
#include "stdint.h"
#include "io.h"
#include "printk.h"

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
        default: return '?';
    }
}

void handle_backspace() {
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;

    if (COL > 0) {
        COL--;
        vga[ROW * VGA_WIDTH + COL] = ' ' | VGA_COLOR;
    } 
    else if (ROW > START_PRINT) {
        ROW--;
        COL = 0;
        for (int col = 79; col >= 0; col--) {
            char ch = vga[ROW * VGA_WIDTH + col] & 0x00FF;
            if (ch != ' ' && ch != 0) {
                COL = col + 1;
                break;
            }
        }
    }
    move_cursor();
}

void    print_keyboard(char c) {
    if (c == '\n') {
        COL = 0;
        ROW++;
        printk(1, "ROW before space : %d\n", ROW);
        check_col();
        move_cursor();
        printk(1, "ROW adter space : %d\n", ROW);
    }
    else if (c == '\b') {
        handle_backspace();
    }
    else
        print_char(c);
    move_cursor();
}

void    keyboard_loop() {
    while(1) {
        unsigned char sc = read_keyboard();
        if (sc & 0x80)      // keyboard relased catch
            continue;
        char c = scancode_to_ascii(sc);
        print_keyboard(c);
    }
}