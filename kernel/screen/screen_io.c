#include <screen.h>
#include <color.h>
#include <io.h>

void screen_putchar(char c, int id) {
    screen_t         *s     = &scr.screens[id];
    int               width = get_width();
    volatile uint16_t *vga  = (uint16_t *)VGA_ADDR;
    uint16_t          color = get_screen_color(id);

    if (c == '\n') 
        s->col = 0, s->row++;
    else {
        s->buffer[s->row * VGA_WIDTH + s->col] = c;

        if (s->flags & SCR_RENDERED) {
            int pos = get_vga_pos(id, s->row, s->col);
            vga[pos] = (uint16_t)c | color;
        }
        s->col++;
        if (s->col >= width)
            s->col = 0, s->row++;
    }

    if (s->row >= VGA_HEIGHT) {
        scroll(id);
        if (scr.mode == SCR_MODE_SPLIT)
            split_refresh(scr.split_left, scr.split_right);
        else
            screen_refresh();
    }

    update_cursor();
}

void screen_backspace() {
    screen_t         *s     = &scr.screens[scr.current];
    int               width = get_width();
    volatile uint16_t *vga  = (uint16_t *)VGA_ADDR;
    uint16_t          color = get_screen_color(scr.current);

    if (s->col > 0) {
        s->col--;
    } else if (s->row > s->start_row) {
        s->row--;
        for (s->col = width - 1; s->col > 0; s->col--) {
            if (s->buffer[s->row * VGA_WIDTH + s->col] != ' ') {
                s->col++;
                break;
            }
        }
        if (s->col == 0 && s->buffer[s->row * VGA_WIDTH] == ' ')
            s->col = 0;
    }

    s->buffer[s->row * VGA_WIDTH + s->col] = ' ';
    int pos = get_vga_pos(scr.current, s->row, s->col);
    vga[pos] = ' ' | color;

    update_cursor();
}
