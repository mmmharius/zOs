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

static int buf_used(screen_t *s) {
    int used = 0;
    int total = VGA_WIDTH * VGA_HEIGHT;
    for (int i = 0; i < total; i++)
        if (s->buffer[i] != ' ')
            used++;
    return used;
}

static void flags_str(uint8_t f, char *out) {
    int i = 0;
    if (f & SCR_ACTIVE)   
        out[i++] = 'A';
    if (f & SCR_RENDERED) 
        out[i++] = 'R';
    if (f & SCR_DEBUG)    
        out[i++] = 'D';
    if (f & SCR_SPLIT_L)  
        out[i++] = 'L';
    if (f & SCR_SPLIT_R)  
        out[i++] = 'R';
    out[i] = '\0';
}

/* ── debug_print_state ────────────────────────────────────────────── */
/*
   Layout (rows 0–7 of debug screen, 40 cols in split mode):

   row 0: ╔═[zOs DEBUG]══════════════╗
   row 1: ║ KEY 'a'  sc=0x1E  asc=97 ║
   row 2: ║ SCR cur=0  SPLIT  L=0 R=4║
   row 3: ║ POS row=7 col=3 w=40     ║
   row 4: ║ FLG [0]=AR [1]=A [2]=AD  ║
   row 5: ║ BUF used=320 free=1680   ║
   row 6: ║ HST h e l l o z o s >    ║
   row 7: ╚══════════════════════════╝
*/

void debug_print_state(unsigned char sc) {
    screen_t *dbg   = &scr.screens[DEBUG_SCREEN_ID];
    int       saved = scr.current;
    char      key   = scancode_to_ascii(sc);

    char hst_char = (key && key != '\n' && key != '\b') ? key : '?';
    if (dbg_history_count < DBG_HISTORY_SIZE)
        dbg_key_history[dbg_history_count++] = hst_char;
    else {
        ft_memmove(dbg_key_history, dbg_key_history + 1, DBG_HISTORY_SIZE - 1);
        dbg_key_history[DBG_HISTORY_SIZE - 1] = hst_char;
    }

    dbg->row = 0;
    dbg->col = 0;
    ft_memset(dbg->buffer, ' ', 8 * VGA_WIDTH);

    scr.current = DEBUG_SCREEN_ID;

    printk(0, "+=[ zOs DEBUG ]================+\n");

    char disp = (key && key != '\n' && key != '\b') ? key : '?';
    printk(0, "| KEY '%c'  sc=0x%x  asc=%d\n", disp, sc, (int)key);

    const char *mode_str = (scr.mode == SCR_MODE_SPLIT) ? "SPLIT" : "NORM ";
    printk(0, "| SCR cur=%d  %s  L=%d R=%d\n",
        saved, mode_str, scr.split_l, scr.split_r);

    int width = (scr.mode == SCR_MODE_SPLIT) ? VGA_WIDTH / 2 : VGA_WIDTH;
    printk(0, "| POS row=%d col=%d  w=%d\n",
        scr.screens[saved].row, scr.screens[saved].col, width);

    char fs[MAX_SCREENS][6];
    for (int i = 0; i < MAX_SCREENS; i++)
        flags_str(scr.screens[i].flags, fs[i]);
    printk(0, "| FLG [0]=%s [1]=%s [2]=%s [3]=%s\n",
        fs[0], fs[1], fs[2], fs[3]);

    int used  = buf_used(&scr.screens[saved]);
    int total = VGA_WIDTH * VGA_HEIGHT;
    printk(0, "| BUF used=%d free=%d\n", used, total - used);

    printk(0, "| HST");
    for (int i = 0; i < dbg_history_count; i++)
        printk(0, " %c", dbg_key_history[i]);
    printk(0, "\n");

    printk(0, "+==============================+\n");
    
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

    /* clear print zone (rows 8 to end) */
    ft_memset(dbg->buffer + 8 * VGA_WIDTH, ' ', (VGA_HEIGHT - 8) * VGA_WIDTH);

    /* copy rows */
    int copy_rows = max_rows;
    if (copy_rows > VGA_HEIGHT - 8)
        copy_rows = VGA_HEIGHT - 8;

    for (int row = 0; row < copy_rows; row++)
        ft_memcpy(
            dbg->buffer + (8 + row) * VGA_WIDTH,
            src->buffer + row * VGA_WIDTH,
            VGA_WIDTH / 2   /* only left half visible in split */
        );

    if (!(scr.mode == SCR_MODE_SPLIT &&
          (scr.screens[scr.split_r].flags & SCR_DEBUG))) {
        screen_open_split(DEBUG_SCREEN_ID);
    }

    split_refresh(scr.split_l, scr.split_r);
}