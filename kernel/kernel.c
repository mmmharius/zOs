#include "kernel.h"
#include "keyboard.h"
#include "libasm/libasm.h"
#include <sys/io.h>

int COL = 0;
int LIN = 0;

void    print_42() {
    volatile uint16_t* vga = (uint16_t*)0xB8000; // 0xB8000 -> VGA TEXT mode video memory address

    vga[LIN + COL * 80] = (uint16_t)'4' | 0x0F00;  // each vga[i] is 2 bytes = 16 bits
    LIN++;
    vga[LIN + COL * 80] = (uint16_t)'2' | 0x0F00;
    LIN++;
}

unsigned char read_keyboard() {
    while ((inb(KB_STATUS) & 1) == 0);
        return inb(KB_DATA);
}

char scancode_to_ascii(unsigned char sc) {
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

void    print_char(char c) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    if (LIN == 80) {
        COL++;
        LIN = 0;
    }
    vga[LIN + COL * 80] = (uint16_t)c | 0x0F00;
    LIN++;
}

void keyboard_loop() {
    while(1) {
        unsigned char sc = read_keyboard();
        char c = scancode_to_ascii(sc);
        print_char(c);
    }
}




/*
    vga[0] = (uint16_t)'4' | 0x0F00

    (uint16_t)'4' in ASCII  = 0x0034
    attribute (white/black) = 0x0F00
    result                  = 0x0F34

    0x0F34 = 0000 1111 0011 0100
             └┬─┘ └┬─┘ └───┬───┘
              │    │       └─ ASCII character ('4')
              │    └── foreground color of text
              └── color of screen behind text

    Foreground (lower 4 bits of attribute):
    0xF (or 1111) → white text

    Background (upper 4 bits of attribute):
    0x0 (or 0000) → black background

    Memory (little endian):
    [0xB8000] = 0x34  // low byte character 
    [0xB8001] = 0x0F  // high byte attribute
*/

void main() {
    // print_42();
    keyboard_loop();
}
