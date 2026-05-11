#ifndef DEBUG_H
#define DEBUG_H

#include <color.h>
#include <keyboard.h>
#include <screen.h>
#include <stdint.h>
#include <io.h>
#include <printk.h>

#define DBG_HISTORY_SIZE 10

void    print_all_screen();
void    print_current_screen();
void    print_screen(int id);
void    debug_print_state(unsigned char sc);

#endif