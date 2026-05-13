#include "debug.h"
#include <libc.h>

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

void debug_print_state(unsigned char sc) {
    screen_t *dbg         = &scr.screens[DEBUG_SCREEN_ID];
    int       saved       = scr.current;
    char      key_pressed = scancode_to_ascii(sc);

    if (!key_pressed)
        key_pressed = '?';
    if (key_pressed == '\n')
        key_pressed = '_';

    if (dbg_history_count < DBG_HISTORY_SIZE)
        dbg_key_history[dbg_history_count++] = key_pressed;
    else {
        ft_memmove(dbg_key_history, dbg_key_history + 1, DBG_HISTORY_SIZE - 1);
        dbg_key_history[DBG_HISTORY_SIZE - 1] = key_pressed;
    }

    dbg->row = 0;
    dbg->col = 0;
    ft_memset(dbg->buffer, ' ', (1 + DBG_HISTORY_SIZE) * VGA_WIDTH);

    scr.current = DEBUG_SCREEN_ID;

    printk(0, "[KEY ] key='%c' sc=0x%x\n", key_pressed, sc);
    printk(0, "[SCR ] cur=%d mode=%d L=%d R=%d\n",
        saved, scr.mode, scr.split_l, scr.split_r);
    printk(0, "[POS ] row=%d col=%d\n",
        scr.screens[saved].row, scr.screens[saved].col);
    printk(0, "[FLG ] 0=%x 1=%x 2=%x 3=%x\n",
        scr.screens[0].flags, scr.screens[1].flags,
        scr.screens[2].flags, scr.screens[3].flags);
    printk(0, "[HST ]");
    for (int i = 0; i < dbg_history_count; i++)
        printk(0, " %c", dbg_key_history[i]);
    printk(0, "\n");

    scr.current = saved;

    split_refresh(scr.split_l, scr.split_r);
}

void debug_live_print(const char **arg) {
    if (!arg) {
        printk(SERIAL, "[PRINT] no arg — usage: PRINT [0-4] [0-1]\n");
        printk(SERIAL, "[PRINT] help if you want");
        return;
    }
    if (a)
    if (arg[0] < '0' || arg[0] > '4' || arg[1] != '\0'  || arg[1] < '0' || arg[2] > '1' || arg[3] != '\0') {
        printk(0, "[PRINT] unknown arg: %s\n", arg);
        return;
    }

    int       id  = arg[0] - '0';
    screen_t *dbg = &scr.screens[DEBUG_SCREEN_ID];
    screen_t *src = &scr.screens[id];


    // copie le contenu du screen demandé dans le debug screen, row par row
    // 10 cols seulement — c'est tout ce qui est visible en split
    for (int row = 0; row < VGA_HEIGHT - 5 && row < VGA_HEIGHT; row++)
        ft_memcpy(dbg->buffer + (5 + row) * VGA_WIDTH, src->buffer + row * VGA_WIDTH, VGA_WIDTH / 2);

    // ouvre le split avec debug screen à droite (read-only, jaune)
    screen_open_split(DEBUG_SCREEN_ID);
    split_refresh(scr.split_l, scr.split_r);
}