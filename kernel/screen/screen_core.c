#include <screen.h>
#include <color.h>
#include <io.h>
#ifdef DEBUG
    #include <printk.h>
#endif

screen_mgr_t scr;


void screen_init() {
    for (int i = 0; i < MAX_SCREENS; i++) {
        scr.screens[i].row          = 0;
        scr.screens[i].col          = 0;
        scr.screens[i].start_row    = (i == 0) ? 5 : (i == DEBUG_SCREEN_ID) ? 4 : 0;
        scr.screens[i].flags        = SCR_ACTIVE;
        for (int j = 0; j < VGA_WIDTH * VGA_HEIGHT; j++)
            scr.screens[i].buffer[j] = ' ';
    }
    scr.count               = MAX_SCREENS;
    scr.current             = 0;
    scr.mode                = SCR_MODE_NORMAL;
    scr.split_left          = 0;
    scr.split_right         = 1;
    scr.screens[0].flags   |= SCR_RENDERED;
    screen_refresh();
}

void screen_switch(int id) {
    if (id < 0 || id >= MAX_SCREENS || !(scr.screens[id].flags & SCR_ACTIVE)) {
        #ifdef DEBUG
            printk(1, "erreur screen switch\n");
        #endif
        return;
    }
    if (scr.mode == SCR_MODE_SPLIT) {
        split_refresh(scr.split_left, scr.split_right);
    } else {
        scr.screens[scr.current].flags &= ~SCR_RENDERED;
        scr.current = id;
        scr.screens[scr.current].flags |= SCR_RENDERED;
        screen_refresh();
    }
}
