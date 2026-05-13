#include <screen.h>
#include <color.h>
#include <io.h>
#include <libc.h>
#ifdef DEBUG
    #include <printk.h>
#endif

screen_mgr_t scr;


void screen_init() {
    for (int i = 0; i < MAX_SCREENS; i++) {
        scr.screens[i].row       = 0;
        scr.screens[i].col       = 0;
        #ifdef DEBUG
            scr.screens[i].start_row = (i == 0) ? 1 : (i == DEBUG_SCREEN_ID) ? 5 : 0;
            scr.screens[i].flags = SCR_ACTIVE | (i == DEBUG_SCREEN_ID ? SCR_DEBUG : 0);
        #else
            scr.screens[i].start_row = (i == 0) ? 5 : 0;
            scr.screens[i].flags = SCR_ACTIVE;
        #endif
        ft_memset(scr.screens[i].buffer, ' ', VGA_WIDTH * VGA_HEIGHT);
    }
    scr.count               = MAX_SCREENS;
    scr.current             = 0;
    scr.mode                = SCR_MODE_NORMAL;
    scr.split_l             = 0;
    scr.split_r             = 1;
    scr.screens[0].flags   |= SCR_RENDERED;
    screen_refresh();
}

void screen_switch(int id) {
    if (id < 0 || id >= MAX_SCREENS || !(scr.screens[id].flags & SCR_ACTIVE)) {
        return;
    }
    if (scr.mode == SCR_MODE_SPLIT) {
        split_refresh(scr.split_l, scr.split_r);
    } else {
        scr.screens[scr.current].flags &= ~SCR_RENDERED;
        scr.current = id;
        scr.screens[scr.current].flags |= SCR_RENDERED;
        screen_refresh();
    }
}
