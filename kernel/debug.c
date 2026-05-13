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
            for (int i = 0; i < DBG_HISTORY_SIZE - 1; i++)
                dbg_key_history[i] = dbg_key_history[i + 1];
            dbg_key_history[DBG_HISTORY_SIZE - 1] = key_pressed;
        }
        if (DBG_G_MODE == 0) {
            dbg->row = 0;
            dbg->col = 0;

            clear here
        
            scr.current = DEBUG_SCREEN_ID;
        
            printk(0, "[KEY PRESS]| key=\'%c\' sc=0x%x\n", key_pressed, sc);
            printk(0, "[SCREEN]   | cur=%d mode=%d sp-L=%d sp-R=%d\n",
                saved, scr.mode, scr.split_left, scr.split_right);
            printk(0, "[CURRENT]  | row=%d col=%d\n",
                scr.screens[saved].row, scr.screens[saved].col);
            printk(0, "[FLAG]     | scr_0=%x scr_1=%x scr_2=%x\n",
                scr.screens[0].flags, scr.screens[1].flags, scr.screens[2].flags);
            printk(0, "[HISTORY]  |");
            for (int i = 0; i < dbg_history_count; i++)
                printk(0, " %c", dbg_key_history[i]);
            printk(0, "\n");
        
            scr.current = saved;
        
            split_refresh(scr.split_left, scr.split_right);
        }
        else if (DBG_G_MODE == 1) {
            dbg->row = 0;
            dbg->col = 0;
            for (int row = 0; row < 4 + DBG_HISTORY_SIZE; row++)
                replace_row(row, DEBUG_SCREEN_ID);
        
            scr.current = DEBUG_SCREEN_ID;
        
            printk(0, "[KEY PRESS]| key=%c sc=0x%x\n", key_pressed, sc);
            printk(0, "[SCREEN]   | cur=%d mode=%d sp-L=%d sp-R=%d\n",
                saved, scr.mode, scr.split_left, scr.split_right);
            printk(0, "[CURRENT]  | row=%d col=%d\n",
                scr.screens[saved].row, scr.screens[saved].col);
            printk(0, "[FLAG]     | scr_0=%x scr_1=%x scr_2=%x\n",
                scr.screens[0].flags, scr.screens[1].flags, scr.screens[2].flags);
            printk(0, "[HISTORY]  |");
            for (int i = 0; i < dbg_history_count; i++)
                printk(0, " %c", dbg_key_history[i]);
            printk(0, "\n");
        
            scr.current = saved;
        
            split_refresh(scr.split_left, scr.split_right);
        }
    }

    void    debug_live_print() {
        
    }