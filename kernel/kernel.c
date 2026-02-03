#include "kernel.h"
#include "io.h"
#include "keyboard.h"
#include "stdint.h"

int COL = 0;
int ROW = 0;


unsigned char read_keyboard() {
    while ((inb(KB_STATUS) & 1) == 0);
    return inb(KB_DATA);
}

void    check_col() {
    if (COL >= 80) {
        COL = 0;
        ROW++;
    }
    // if (ROW >= 25) {
    //     //scrol ???
    // }
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

void handle_backspace() {
    volatile uint16_t* vga = (uint16_t*)0xB8000;

    if (COL > 0) {
        COL--;
        vga[ROW * 80 + COL] = ' ' | 0x0F00;
    } else if (ROW > 0) {
        ROW--;
        int last_col = 0;
        for (int col = 79; col >= 0; col--) {
            char ch = vga[ROW * 80 + col] & 0x00FF;
            if (ch != 0) {
                last_col = col; 
                break;
            }
        }
        COL = last_col;
    }
    move_cursor();
}

void    print_keyboard(char c) {
    if (c == '\n') {
        COL = 0;
        ROW++;
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

void    main() {
    print_42();
    keyboard_loop();
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