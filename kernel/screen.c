#include <keyboard.h>
#include <screen.h>
#include <stdint.h>
#include <io.h>
#include <printk.h>
#include <color.h>

void replace_entire_row(int row, char c)
{
    printk(1, "REPLACE CALL ROW : %d", row);
    volatile uint16_t *vga = (uint16_t *)VGA_ADDR;

    for (int col = 0; col < VGA_WIDTH; col++) {
        vga[row * VGA_WIDTH + col] =
            (uint16_t)c | VGA_DEFAULT_COLOR;
    }
}

void    scroll() {
    printk(1, "SCROLL CALL");
    volatile uint16_t *vga = (uint16_t *)VGA_ADDR;

    for (int row = START_PRINT + 1; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            vga[(row - 1) * VGA_WIDTH + col] = vga[row * VGA_WIDTH + col];
        }
    }
    current->row = VGA_HEIGHT - 1;
    current->col = 0;
    printk(1, "row before call replace:%d\n", current->row);
    replace_entire_row(current->row, ' ');
    printk(1, "row after call replace:%d\n", current->row);
}

void    check_col() {
    int current_id = current - screens;
    printk(1, "CHECK CALL START\n COL: %d ROW: %d SCREEN:%d, current id:%d\n", current->col, current->row, SCREEN, current_id);
    if (current->col >= VGA_WIDTH) {
        current->col = 0;
        current->row++;
    }
    if (current->row >= VGA_HEIGHT)
        scroll();
}

void    print_char(char c) {
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR; // 0xB8000 -> VGA TEXT mode video memory address
    check_col();
    vga[current->col + current->row * VGA_WIDTH] = (uint16_t)c | VGA_DEFAULT_COLOR;
    current->col++;
    move_cursor();
}

void    print_42() {
    print_char('4');
    print_char('2');
}


void move_cursor() {
    uint16_t pos = current->row * VGA_WIDTH + current->col;
    
    outb(0x3D4, 0x0F); // 0x0F  = cursor low byte
    // 0x3D4 = choose register | 
    outb(0x3D5, (uint8_t)(pos & 0xFF)); 
    outb(0x3D4, 0x0E);  // 0x0E  = cursor high byte
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void init_screen() {
    for (int i = 0; i < (NB_SCREEN - 1); i++) {
        screens[i].row = 0;
        screens[i].col = 0;
        screens[i].scroll = 0;

        for (int j = START_PRINT; j < VGA_WIDTH * VGA_HEIGHT; j++)
            screens[i].buffer[j] = (uint16_t)' ' | VGA_DEFAULT_COLOR;
    }
    current = &screens[0];
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