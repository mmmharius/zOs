#include "debug.h"

static char     dbg_key_history[DBG_HISTORY_SIZE];
static unsigned char dbg_sc_history[DBG_HISTORY_SIZE];
static int      dbg_history_count = 0;

void print_screen(int id) {
    printk(SERIAL, "SCREEN : %d\n\n", id);
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++)
            printk(SERIAL, "%c", scr.screens[id].buffer[row * VGA_WIDTH + col]);
        printk(SERIAL, "\n");
    }
}

void print_all_screen() {
    for (int i = 0; i < scr.count; i++)
        print_screen(i);
}

void print_current_screen() {
    print_screen(scr.current);
}

void debug_print_state(unsigned char sc) {
    screen_t *dbg = &scr.screens[DEBUG_SCREEN_ID];

    char key_pressed = scancode_to_ascii(sc);
    if (!key_pressed) key_pressed = '?';

    if (dbg_history_count < DBG_HISTORY_SIZE) {
        dbg_key_history[dbg_history_count] = key_pressed;
        dbg_sc_history[dbg_history_count]  = sc;
        dbg_history_count++;
    } else {
        for (int i = 0; i < DBG_HISTORY_SIZE - 1; i++) {
            dbg_key_history[i] = dbg_key_history[i + 1];
            dbg_sc_history[i]  = dbg_sc_history[i + 1];
        }
        dbg_key_history[DBG_HISTORY_SIZE - 1] = key_pressed;
        dbg_sc_history[DBG_HISTORY_SIZE - 1]  = sc;
    }

    dbg->row = 0, dbg->col = 0;

    for (int row = 0; row < 4 + DBG_HISTORY_SIZE; row++)
        replace_row(row, DEBUG_SCREEN_ID);

    printk(DBG, "[KEY PRESS]| key=%c sc=0x%x\n", key_pressed, sc);
    printk(DBG, "[SCREN]    | cur=%d mode=%d sp-L=%d sp-R=%d\n",
        scr.current, scr.mode, scr.split_left, scr.split_right);
    printk(DBG, "[CURRENT]  | row=%d col=%d\n",
        scr.screens[scr.current].row, scr.screens[scr.current].col);
    printk(DBG, "[FLAG]     | scr_0=%x scr_1=%x scr_2=%x\n",
        scr.screens[0].flags, scr.screens[1].flags, scr.screens[2].flags);

    printk(DBG, "[HISTORY]  |");
    for (int i = 0; i < dbg_history_count; i++)
        printk(DBG, " %c", dbg_key_history[i]);
    printk(DBG, "\n");

    if (dbg->flags & SCR_RENDERED)
        split_refresh(scr.split_left, scr.split_right);
}
