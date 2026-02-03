#ifndef SCREEN_H
#define SCREEN_H

#define VGA_COLOR 0x0F00
#define VGA_ADDR 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void    replace_entire_row(int row, char c);
void    check_col();
void    print_char(char c);
void    print_42();
void    move_cursor();

#endif