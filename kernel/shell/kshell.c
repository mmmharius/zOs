#include <screen.h>
#include <keyboard.h>
#include <stdint.h>
#include <printk.h>
#ifdef DEBUG
    #include <debug.h>
#endif
#include "kshell.h"

#define KSHELL_BUFSIZE 256
#define PROMPT         "zOs> "

static char buf[KSHELL_BUFSIZE];
static int  len = 0;

static int kstrcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2)
        s1++, s2++;
    return (unsigned char)*s1 - (unsigned char)*s2;
}

static void screen_clear_current(void) {
    screen_t *s     = &scr.screens[scr.current];
    int       width = get_width();

    for (int row = s->start_row; row < VGA_HEIGHT; row++)
        for (int col = 0; col < width; col++)
            s->buffer[row * VGA_WIDTH + col] = ' ';
    s->row = s->start_row;
    s->col = 0;

    if (scr.mode == SCR_MODE_SPLIT) {
        int other = (scr.current == scr.split_left)
            ? scr.split_right : scr.split_left;
        screen_t *s2 = &scr.screens[other];
        for (int row = s2->start_row; row < VGA_HEIGHT; row++)
            for (int col = 0; col < width; col++)
                s2->buffer[row * VGA_WIDTH + col] = ' ';
        s2->row = s2->start_row;
        s2->col = 0;
        split_refresh(scr.split_left, scr.split_right);
    } else
        screen_refresh();
}

static void kshell_exec(char *line) {
    if (kstrcmp(line, "CLEAR") == 0)
        screen_clear_current();
    else if (kstrcmp(line, "HELP") == 0)
        printk(0, "commands: CLEAR HELP\n");
}

static void kshell_handler(char c) {
    if (c == '\n') {
        screen_putchar('\n', scr.current);
        buf[len] = '\0';
        len = 0;
        kshell_exec(buf);
        printk(0, PROMPT);
    } else if (c == '\b') {
        if (len > 0) {
            len--;
            screen_backspace();
        }
    } else {
        if (len < KSHELL_BUFSIZE - 1) {
            buf[len++] = c;
            screen_putchar(c, scr.current);
        }
    }
}

void kshell_run(void) {
    printk(0, PROMPT);
    keyboard_loop(kshell_handler);
}