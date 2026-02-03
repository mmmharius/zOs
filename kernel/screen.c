#include "screen.h"
#include "stdint.h"
#include "keyboard.h"
#include "kernel.h"
#include "io.h"
#include "printk.h"

void replace_entire_row(int row, char c)
{
    printk(1, "REPLACE CALL ROW : %d", row);
    volatile uint16_t *vga = (uint16_t *)VGA_ADDR;

    for (int col = 0; col < VGA_WIDTH; col++) {
        vga[row * VGA_WIDTH + col] =
            (uint16_t)c | VGA_COLOR;
    }
}

void    scroll() {
    printk(1, "SCROLL CALL");
    volatile uint16_t *vga = (uint16_t *)VGA_ADDR;

    for (int row = 1; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vga[(row - 1) * VGA_WIDTH + col] = vga[row * VGA_WIDTH + col];
        }
    }
    ROW = VGA_HEIGHT - 1;
    COL = 0;
    printk(1, "row before call replace:%d\n", ROW);
    replace_entire_row(ROW, ' ');
    printk(1, "row after call replace:%d\n", ROW);
}

void    check_col() {
    printk(1, "CHECK CALL START\n COL: %d ROW: %d\n", COL, ROW);
    if (COL >= VGA_WIDTH) {
        COL = 0;
        ROW++;
    }
    if (ROW >= 25)
        scroll();
}

void    print_char(char c) {
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR; // 0xB8000 -> VGA TEXT mode video memory address
    check_col();
    vga[COL + ROW * VGA_WIDTH] = (uint16_t)c | VGA_COLOR;
    COL++;
    move_cursor();
}

void    print_42() {
    print_char('4');
    print_char('2');
}

void move_cursor()
{
    uint16_t pos = ROW * VGA_WIDTH + COL;
    
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