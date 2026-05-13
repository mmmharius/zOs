#ifndef DEBUG_H
#define DEBUG_H

#include <color.h>
#include <keyboard.h>
#include <screen.h>
#include <stdint.h>
#include <io.h>
#include <printk.h>

#define DBG_HISTORY_SIZE 16

void    print_all_screen(void);
void    print_current_screen(void);
void    print_screen(int id);
void    debug_print_state(unsigned char sc);
void    debug_live_print(int id, int max_rows);

#endif