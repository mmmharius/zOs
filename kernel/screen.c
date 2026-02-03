#include "screen.h"
#include "stdint.h"
#include "keyboard.h"
#include "kernel.h"
#include "io.h"

void    print_char(char c) {
    volatile uint16_t* vga = (uint16_t*)0xB8000; // 0xB8000 -> VGA TEXT mode video memory address
    check_col();
    vga[COL + ROW * 80] = (uint16_t)c | 0x0F00;
    COL++;
    move_cursor();
}

void    print_42() {
    print_char('4');
    print_char('2');
}

void move_cursor()
{
    uint16_t pos = ROW * 80 + COL;
    
    outb(0x3D4, 0x0F); // 0x0F  = cursor low byte
    // 0x3D4 = choose register | 
    outb(0x3D5, (uint8_t)(pos & 0xFF)); 
    outb(0x3D4, 0x0E);  // 0x0E  = cursor high byte
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
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