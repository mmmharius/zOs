#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#define VGA_ADDR 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define NB_SCREEN 2
#define START_PRINT 5
#define SCREEN_LENGHT 1

extern int SCREEN;

typedef struct s_screen {
    uint16_t buffer[VGA_WIDTH * (VGA_HEIGHT * SCREEN_LENGHT)];
    int row;
    int col;
    // int row_max;
    // int col_max;
    // int row_start;
    // int row_start;
    int scroll;
} t_screen;

extern t_screen screens[NB_SCREEN];
extern t_screen *current;

void    replace_entire_row(int row, char c);
void    check_col();
void    print_char(char c);
void    print_42();
void    move_cursor();
void    init_screen();
void    switch_screen(int id);

#endif