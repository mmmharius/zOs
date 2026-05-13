#include <screen.h>
#include <color.h>
#ifdef DEBUG
    #include <printk.h>
#endif

static int saved_col = -1;

static void vga_clear_right_half(void) {
    volatile uint16_t *vga = (uint16_t *)VGA_ADDR;
    for (int row = 0; row < VGA_HEIGHT; row++)
        for (int col = VGA_WIDTH / 2; col < VGA_WIDTH; col++)
            vga[row * VGA_WIDTH + col] = ' ' | 0x0700;
}

static void scr_enter_split(int left, int right, uint8_t right_extra) {
    scr.split_l  = left;
    scr.split_r  = right;
    scr.mode     = SCR_MODE_SPLIT;
    scr.screens[left].flags  |= SCR_SPLIT_L | SCR_RENDERED;
    scr.screens[right].flags |= SCR_SPLIT_R | SCR_RENDERED | right_extra;
    if (scr.screens[left].col >= VGA_WIDTH / 2) {
        saved_col = scr.screens[left].col;
        scr.screens[left].col = VGA_WIDTH / 2 - 1;
    } else {
        saved_col = -1;
    }
    split_refresh(left, right);
}

static void scr_exit_split(void) {
    scr.screens[scr.split_l].flags &= ~(SCR_SPLIT_L | SCR_RENDERED);
    scr.screens[scr.split_r].flags &= ~(SCR_SPLIT_R | SCR_RENDERED);
    scr.screens[scr.current].flags |= SCR_RENDERED;
    scr.mode = SCR_MODE_NORMAL;
    if (saved_col != -1) {
        scr.screens[scr.current].col = saved_col;
        saved_col = -1;
    }
    vga_clear_right_half();
    screen_refresh();
}

static void vga_draw_screen(screen_t *s, int id, int col_offset) {
    volatile uint16_t *vga = (uint16_t *)VGA_ADDR;
    uint16_t color = get_screen_color(id);

    int width = (scr.mode == SCR_MODE_SPLIT) ? VGA_WIDTH / 2 : VGA_WIDTH;

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

void screen_refresh(void) {
    vga_draw_screen(&scr.screens[scr.current], scr.current, 0);
    update_cursor();
}

void screen_toggle_split(void) {
    if (scr.mode == SCR_MODE_NORMAL) {
        int right = (scr.current + 1) % MAX_SCREENS;
        #ifdef DEBUG
            if (right == DEBUG_SCREEN_ID)
                right = (right + 1) % MAX_SCREENS;
        #endif
        scr_enter_split(scr.current, right, 0);
    } else
        scr_exit_split();
}

void screen_open_split(int right_id) {
    if (scr.mode == SCR_MODE_SPLIT)
        scr_exit_split();
    int left = scr.current;
    scr_enter_split(left, right_id, SCR_DEBUG);
    scr.current = left;
    update_cursor();
}

void screen_close_split(void) {
    if (scr.mode != SCR_MODE_SPLIT)
        return;
    scr_exit_split();
}

#ifdef DEBUG
    void screen_toggle_debug_split(void) {
        if (scr.mode == SCR_MODE_NORMAL)
            scr_enter_split(scr.current, DEBUG_SCREEN_ID, SCR_DEBUG);
        else
            scr_exit_split();
    }
#endif