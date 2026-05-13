#include <screen.h>
#include <io.h>
#include <libc.h>
#ifdef DEBUG
    #include <printk.h>
    #include <debug.h>
#endif

int get_width() {
    return (scr.mode == SCR_MODE_SPLIT) ? VGA_WIDTH / 2 : VGA_WIDTH;
}

int get_vga_pos(int id, int row, int col) {
    if (scr.mode == SCR_MODE_SPLIT) {
        int offset = (id == scr.split_r) ? VGA_WIDTH / 2 : 0;
        return row * VGA_WIDTH + col + offset;
    }
    return row * VGA_WIDTH + col;
}

void update_cursor() {
    screen_t *s   = &scr.screens[scr.current];
    int       pos = get_vga_pos(scr.current, s->row, s->col);

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void replace_row(int row, int id) {
    ft_memset(&scr.screens[id].buffer[row * VGA_WIDTH], ' ', VGA_WIDTH);
}

void scroll(int id) {
    screen_t *s     = &scr.screens[id];
    int protected   = s->start_row * VGA_WIDTH;
    int total       = VGA_WIDTH * VGA_HEIGHT;

    ft_memmove(s->buffer + protected,s->buffer + protected + VGA_WIDTH, total - protected - VGA_WIDTH);
    ft_memset(s->buffer + total - VGA_WIDTH, ' ', VGA_WIDTH); 
    s->row = VGA_HEIGHT - 1;
    s->col = 0;
}
