#include <screen.h>
#include <color.h>
#ifdef DEBUG
    #include <printk.h>
#endif

static void vga_draw_screen(screen_t *s, int id, int col_offset) {
    volatile uint16_t *vga = (uint16_t *)VGA_ADDR;
    uint16_t           color = get_screen_color(id);
    int                width = col_offset ? VGA_WIDTH / 2 : VGA_WIDTH;

    for (int row = 0; row < VGA_HEIGHT; row++)
        for (int col = 0; col < width; col++)
            vga[row * VGA_WIDTH + col + col_offset] =
                (uint16_t)s->buffer[row * VGA_WIDTH + col] | color;
}

void split_refresh(int left_id, int right_id) {
    vga_draw_screen(&scr.screens[left_id],  left_id,  0);
    vga_draw_screen(&scr.screens[right_id], right_id, VGA_WIDTH / 2);
    update_cursor();
}

void screen_refresh() {
    vga_draw_screen(&scr.screens[scr.current], scr.current, 0);
    update_cursor();
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

void screen_toggle_split() {
    if (scr.mode == SCR_MODE_NORMAL) {
        scr.split_left  = scr.current;
        scr.split_right = (scr.current + 1) % MAX_SCREENS;
        scr.mode        = SCR_MODE_SPLIT;
        scr.screens[scr.split_left].flags  |= SCR_SPLIT_L | SCR_RENDERED;
        scr.screens[scr.split_right].flags |= SCR_SPLIT_R | SCR_RENDERED;
        split_refresh(scr.split_left, scr.split_right);
    } else {
        scr.screens[scr.split_left].flags  &= ~(SCR_SPLIT_L | SCR_RENDERED);
        scr.screens[scr.split_right].flags &= ~(SCR_SPLIT_R | SCR_RENDERED);
        scr.mode        = SCR_MODE_NORMAL;
        scr.screens[scr.current].flags |= SCR_RENDERED;
        screen_refresh();
    }
}
