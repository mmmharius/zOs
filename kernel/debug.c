#include "debug.h"

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

void print_all_screen() {
    for (int i = 0; i < scr.count; i++)
        print_screen(i);
}

void print_current_screen() {
    print_screen(scr.current);
}

void debug_print_state(unsigned char sc) {
    int  saved = scr.current;
    char key   = scancode_to_ascii(sc);

    if (!key)
        key = '?';

    if (dbg_history_count < DBG_HISTORY_SIZE)
        dbg_key_history[dbg_history_count++] = key;
    else {
        for (int i = 0; i < DBG_HISTORY_SIZE - 1; i++)
            dbg_key_history[i] = dbg_key_history[i + 1];
        dbg_key_history[DBG_HISTORY_SIZE - 1] = key;
    }

    for (int row = 0; row < 5; row++)
        for (int col = 0; col < VGA_WIDTH; col++)
            scr.screens[DEBUG_SCREEN_ID].buffer[row * VGA_WIDTH + col] = ' ';

    scr.screens[DEBUG_SCREEN_ID].row = 0;
    scr.screens[DEBUG_SCREEN_ID].col = 0;

    printk(DBG, "[KEY ] key=%c sc=0x%x\n", key, sc);
    printk(DBG, "[SCR ] cur=%d mode=%d L=%d R=%d\n",
        saved, scr.mode, scr.split_l, scr.split_r);
    printk(DBG, "[POS ] row=%d col=%d\n",
        scr.screens[saved].row, scr.screens[saved].col);
    printk(DBG, "[FLG ] 0=%x 1=%x 2=%x 3=%x\n",
        scr.screens[0].flags, scr.screens[1].flags,
        scr.screens[2].flags, scr.screens[3].flags);
    printk(DBG, "[HST ]");
    for (int i = 0; i < dbg_history_count; i++)
        printk(DBG, " %c", dbg_key_history[i]);
    printk(DBG, "\n");

    split_refresh(scr.split_l, scr.split_r);
}