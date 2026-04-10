#include <screen.h>
#include <keyboard.h>
#include <stdint.h>
#include <printk.h>
#ifdef DEBUG
    #include <debug.h>
#endif
#include "kshell.h"
#include <kernel.h>

#define KSHELL_BUFSIZE 256
#define PROMPT         "zOs> "

static int kstrcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
        s1++, s2++;
    return (unsigned char)*s1 - (unsigned char)*s2;
}

static void screen_clear_current(void)
{
    screen_t *s = &scr.screens[scr.current];

    for (int row = s->start_row; row < VGA_HEIGHT; row++)
        for (int col = 0; col < VGA_WIDTH; col++)
            s->buffer[row * VGA_WIDTH + col] = ' ';
    s->row = s->start_row;
    s->col = 0;
    screen_refresh();
}

static char *kgnl_readline(void)
{
    static char buf[KSHELL_BUFSIZE];
    static int  len = 0;

    while (1)
    {
        unsigned char sc  = read_keyboard();
        unsigned char key = sc & 0x7F;

        if (key == KEY_CTRL) {
            ctrl_pressed = !(sc & KEY_RELEASE);
            continue;
        }
        if (sc & KEY_RELEASE)
            continue;

        if (ctrl_pressed && key == KEY_G) {
            #ifdef DEBUG
                screen_toggle_split();
            #else
                printk(0, "debug mode not active : make debug for on...\n");
            #endif
            continue;
        }

        if (key == KEY_TAB) {
            if (scr.mode == SCR_MODE_SPLIT) {
                int other = scr.split_right == scr.current
                    ? scr.split_left : scr.split_right;
                if (!(scr.screens[other].flags & SCR_DEBUG))
                    scr.current = other, update_cursor();
            } else
                screen_switch((scr.current + 1) % MAX_SCREENS);
            continue;
        }

        if (key == KEY_1) {
            screen_toggle_split();
            continue;
        }

        char c = scancode_to_ascii(sc);
        #ifdef DEBUG
            debug_print_state(sc);
        #endif
        if (c == 0)
            continue;
        if (c == '\n') {
            screen_putchar('\n', scr.current);
            buf[len] = '\0';
            len = 0;
            return buf;
        }
        if (c == '\b') {
            if (len > 0) {
                len--;
                screen_backspace();
            }
            continue;
        }
        if (len < KSHELL_BUFSIZE - 1) {
            buf[len++] = c;
            screen_putchar(c, scr.current);
        }
    }
}

static void kshell_exec(char *line)
{
    if (kstrcmp(line, "CLEAR") == 0)
        screen_clear_current();
    else if (kstrcmp(line, "HELP") == 0)
        printk(0, "commands: CLEAR HELP\n");
}

void kshell_run(void)
{
    printk(0, PROMPT);
    while (1)
    {
        char *line = kgnl_readline();
        kshell_exec(line);
        printk(0, PROMPT);
    }
}
