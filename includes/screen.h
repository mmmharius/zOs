#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define VGA_ADDR    0xB8000
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define MAX_SCREENS 4

#define SCR_ACTIVE   (1 << 0)
#define SCR_RENDERED (1 << 1)
#define SCR_DEBUG    (1 << 2)
#define SCR_SPLIT_L  (1 << 3)
#define SCR_SPLIT_R  (1 << 4)

#define SCR_MODE_NORMAL 0
#define SCR_MODE_SPLIT  1

#define DEBUG_SCREEN_ID 2

typedef struct {
    int     row;
    int     col;
    int     start_row;
    uint8_t flags;
    char    buffer[VGA_WIDTH * VGA_HEIGHT];
} screen_t;

typedef struct {
    screen_t    screens[MAX_SCREENS];
    int         count;
    int         current;
    int         mode;
    int         split_left;
    int         split_right;
} screen_mgr_t;

extern screen_mgr_t scr;

void    screen_init();
void    screen_switch(int id);
void    screen_toggle_split();
void    screen_putchar(char c, int id);
void    screen_backspace();
void    screen_refresh();

int     get_width();
int     get_vga_pos(int id, int row, int col);
void    update_cursor();
void    replace_row(int row, int id);
void    scroll(int id);
void    split_refresh(int left_id, int right_id);

#ifdef DEBUG
void    toggle_debug_screen();
#endif

#endif
