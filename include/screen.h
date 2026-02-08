#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define VGA_ADDR 0xB8000  // 0xB8000 -> VGA TEXT mode video memory address
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define NB_SCREEN 3

typedef struct {
    char buffer[VGA_WIDTH * VGA_HEIGHT];
    int row;
    int col;
    int start_row;
} screen_t;

extern screen_t screens[NB_SCREEN];
extern int current_screen;
extern int mode_split;

void    screen_init();
void    screen_switch(int id);
void    screen_toggle_split();
void    screen_putchar(char c);
void    screen_backspace();
void    screen_refresh();

#endif