#include "kernel.h"

void    print_42() {
    volatile uint16_t* vga = (uint16_t*)0xB8000; //0xB8000 -> VGA TEXT mode video memory address

    vga[0] = (uint16_t)'4' | 0x0F00;  // each vga[i] is 2 bytes = 16 bits
    vga[1] = (uint16_t)'2' | 0x0F00;
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
    print_42();
}
