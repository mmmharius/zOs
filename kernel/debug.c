#include "debug.h"
#include <libc.h>
#include <printk.h>

static int  dbg_history_count = 0;
static char dbg_key_history[DBG_HISTORY_SIZE];

void print_screen(int id) {
    printk(SERIAL, "SCREEN : %d\n\n", id);
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++)
            printk(SERIAL, "%c", scr.screens[id].buffer[row * VGA_WIDTH + col]);
        printk(SERIAL, "\n");
    }
}

void print_all_screen(void) {
    for (int i = 0; i < scr.count; i++)
        print_screen(i);
}

void print_current_screen(void) {
    print_screen(scr.current);
}

static int buf_used(screen_t *s) {
    int i, used = 0, total = VGA_WIDTH * VGA_HEIGHT;
    for (i = 0; i < total; i++)
        if (s->buffer[i] != ' ')
            used++;
    return used;
}

static void flags_str(uint8_t flag, char *out) {
    int i = 0;
    if (flag  & SCR_ACTIVE)
        out[i++] = 'A';
    if (flag & SCR_RENDERED)
        out[i++] = 'R';
    if (flag & SCR_DEBUG)
        out[i++] = 'D';
    if (flag & SCR_SPLIT_L)
        out[i++] = 'L';
    if (flag & SCR_SPLIT_R)
        out[i++] = 'R';
    out[i] = '\0';
}

static void dbg_write(screen_t *dbg, int row, const char *line) {
    int col = 0;
    int max = VGA_WIDTH / 2;
    while (col < max && line[col]) {
        dbg->buffer[row * VGA_WIDTH + col] = line[col];
        col++;
    }
    while (col < max)
        dbg->buffer[row * VGA_WIDTH + col++] = ' ';
}

void debug_print_state(unsigned char sc) {
    screen_t   *dbg   = &scr.screens[DEBUG_SCREEN_ID];
    int         saved = scr.current;
    char        key   = scancode_to_ascii(sc);
    char        disp  = (key && key != '\n' && key != '\b') ? key : '?';

    if (dbg_history_count < DBG_HISTORY_SIZE)
        dbg_key_history[dbg_history_count++] = disp;
    else {
        ft_memmove(dbg_key_history, dbg_key_history + 1, DBG_HISTORY_SIZE - 1);
        dbg_key_history[DBG_HISTORY_SIZE - 1] = disp;
    }

    ft_memset(dbg->buffer, ' ', 9 * VGA_WIDTH);

    int  w = VGA_WIDTH / 2;
    char line[41];

    ft_memset(line, '=', w);
    line[0] = '+'; line[w - 1] = '+'; line[w] = '\0';
    dbg_write(dbg, 0, line);

    dbg_write(dbg, 1, "|      zOs DEBUG PANEL     |");

    scr.current = DEBUG_SCREEN_ID;

    dbg->row = 2; dbg->col = 0;
    printk(0, "| KEY '%c' sc=0x%x asc=%d\n", disp, sc, (int)key);

    dbg->row = 3; dbg->col = 0;
    const char *mode_str = (scr.mode == SCR_MODE_SPLIT) ? "SPLIT" : "NORM";
    printk(0, "| SCR cur=%d %s L=%d R=%d\n",
           saved, mode_str, scr.split_l, scr.split_r);

    dbg->row = 4; dbg->col = 0;
    int width = (scr.mode == SCR_MODE_SPLIT) ? VGA_WIDTH / 2 : VGA_WIDTH;
    printk(0, "| POS row=%d col=%d w=%d\n",
           scr.screens[saved].row, scr.screens[saved].col, width);

    dbg->row = 5; dbg->col = 0;
    char fs[MAX_SCREENS][6];
    for (int i = 0; i < MAX_SCREENS; i++)
        flags_str(scr.screens[i].flags, fs[i]);
    printk(0, "| FLG 0=%s 1=%s 2=%s 3=%s\n",
           fs[0], fs[1], fs[2], fs[3]);

    dbg->row = 6; dbg->col = 0;
    int used  = buf_used(&scr.screens[saved]);
    int total = VGA_WIDTH * VGA_HEIGHT;
    printk(0, "| BUF used=%d free=%d\n", used, total - used);

    dbg->row = 7; dbg->col = 0;
    printk(0, "| HST");
    for (int i = 0; i < dbg_history_count; i++)
        printk(0, " %c", dbg_key_history[i]);
    printk(0, "\n");

    ft_memset(line, '=', w);
    line[0] = '+'; line[w - 1] = '+'; line[w] = '\0';
    dbg_write(dbg, 8, line);

    dbg->row = 9;
    dbg->col = 0;

    scr.current = saved;
    split_refresh(scr.split_l, scr.split_r);
}

void debug_live_print(int id, int max_rows) {
    if (id < 0 || id >= MAX_SCREENS || id == DEBUG_SCREEN_ID) {
        printk(0, "[PRINT] bad id: %d (valid: 0-%d)\n", id, MAX_SCREENS - 2);
        return;
    }
    if (max_rows <= 0 || max_rows > VGA_HEIGHT)
        max_rows = VGA_HEIGHT;

    screen_t *dbg = &scr.screens[DEBUG_SCREEN_ID];
    screen_t *src = &scr.screens[id];

    int  w = VGA_WIDTH / 2;
    char line[41];

    ft_memset(dbg->buffer, ' ', VGA_WIDTH * VGA_HEIGHT);

    ft_memset(line, '-', w);
    line[0] = '+'; line[w - 1] = '+'; line[w] = '\0';
    dbg_write(dbg, 0, line);

    scr.current = DEBUG_SCREEN_ID;
    dbg->row = 1; dbg->col = 0;
    printk(0, "| LIVE PRINT  scr=%d\n", id);

    ft_memset(line, '-', w);
    line[0] = '+'; line[w - 1] = '+'; line[w] = '\0';
    dbg_write(dbg, 2, line);

    int copy_rows = max_rows;
    if (copy_rows > VGA_HEIGHT - 3)
        copy_rows = VGA_HEIGHT - 3;

    for (int row = 0; row < copy_rows; row++)
        for (int col = 0; col < w; col++)
            dbg->buffer[(3 + row) * VGA_WIDTH + col] =
                src->buffer[row * VGA_WIDTH + col];

    dbg->row = 3 + copy_rows;
    dbg->col = 0;
    scr.current = (scr.split_l != DEBUG_SCREEN_ID) ? scr.split_l : 0;

    if (!(scr.mode == SCR_MODE_SPLIT &&
          (scr.screens[scr.split_r].flags & SCR_DEBUG))) {
        int saved = scr.current;
        screen_open_split(DEBUG_SCREEN_ID);
        scr.current = saved;
        update_cursor();
    }

    split_refresh(scr.split_l, scr.split_r);
}