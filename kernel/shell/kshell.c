#include <screen.h>
#include <keyboard.h>
#include <stdint.h>
#include <printk.h>
#include <libc.h>
#include "kshell.h"
#ifdef DEBUG
    #include <debug.h>
#endif

#define KSHELL_BUFSIZE  256
#define KSHELL_MAX_ARGS 8
#define PROMPT          "zOs> "

static char buf[KSHELL_BUFSIZE];
static int  len = 0;

static int kshell_split(char *line, char **argv, int max)
{
    int argc = 0;
    int i    = 0;

    while (line[i] && argc < max) {
        while (line[i] == ' ')
            i++;
        if (!line[i])
            break;
        argv[argc++] = &line[i];
        while (line[i] && line[i] != ' ')
            i++;
        if (line[i])
            line[i++] = '\0';
    }
    argv[argc] = 0;
    return argc;
}

static void screen_clear_current(void)
{
    screen_t *s     = &scr.screens[scr.current];

    ft_memset(s->buffer + s->start_row * VGA_WIDTH, ' ', (VGA_HEIGHT - s->start_row) * VGA_WIDTH);
    s->row = s->start_row;
    s->col = 0;

    if (scr.mode == SCR_MODE_SPLIT)
        split_refresh(scr.split_l, scr.split_r);
    else
        screen_refresh();
}

static void kshell_exec(char **argv, int argc)
{
    #ifdef DEBUG
        if (ft_strcmp(argv[0], "PRINT") == 0) {
            int id       = (argc > 1) ? ft_atoi(argv[1]) : scr.current;
            int max_rows = (argc > 2) ? ft_atoi(argv[2]) : VGA_HEIGHT;
        debug_live_print(id, max_rows);
    if (argc == 0)
        return;

    if (ft_strcmp(argv[0], "CLEAR") == 0)
        screen_clear_current();

    else if (ft_strcmp(argv[0], "HELP") == 0)
    #ifdef DEBUG
        printk(0, "CLEAR HELP REBOOT GO EXIT PRINT [id] [rows]\n");
    #else
        printk(0, "CLEAR HELP REBOOT GO\n");
    #endif

    else if (ft_strcmp(argv[0], "GO") == 0 || ft_strcmp(argv[0], "EXIT") == 0)
        screen_close_split();

    else if (ft_strcmp(argv[0], "REBOOT") == 0)
        outb(0x64, 0xFE);

    }
#endif

    else
        printk(0, "unknown: %s\n", argv[0]);
}

static void kshell_handler(char c)
{
    if (c == '\n') {
        screen_putchar('\n', scr.current);
        buf[len] = '\0';
        len = 0;
        char *argv[KSHELL_MAX_ARGS + 1];
        int   argc = kshell_split(buf, argv, KSHELL_MAX_ARGS);
        kshell_exec(argv, argc);
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