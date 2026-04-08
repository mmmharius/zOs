#include <keyboard.h>
#include <screen.h>
#include <stdint.h>
#include <io.h>
#ifdef DEBUG
    #include <printk.h>
    #include <debug.h>
#endif

int ctrl_pressed = 0;

unsigned char read_keyboard() {
    while ((inb(KB_STATUS) & 1) == 0);
    return inb(KB_DATA);
}

char scancode_to_ascii(unsigned char sc) {
    switch(sc) {
        case KEY_1: return '1';
        case KEY_Q: return 'Q';
        case KEY_W: return 'W';
        case KEY_E: return 'E';
        case KEY_R: return 'R';
        case KEY_T: return 'T';
        case KEY_Y: return 'Y';
        case KEY_U: return 'U';
        case KEY_I: return 'I';
        case KEY_O: return 'O';
        case KEY_P: return 'P';
        case KEY_A: return 'A';
        case KEY_S: return 'S';
        case KEY_D: return 'D';
        case KEY_F: return 'F';
        case KEY_G: return 'G';
        case KEY_H: return 'H';
        case KEY_J: return 'J';
        case KEY_K: return 'K';
        case KEY_L: return 'L';
        case KEY_Z: return 'Z';
        case KEY_X: return 'X';
        case KEY_C: return 'C';
        case KEY_V: return 'V';
        case KEY_B: return 'B';
        case KEY_N: return 'N';
        case KEY_M: return 'M';
        case KEY_ENTER: return '\n';
        case KEY_BACKSPACE: return '\b';
        case KEY_SPACE: return ' ';
        default: return 0;
    }
}

void keyboard_loop() {
    while (1) {
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
                char *msg = "debug mode not active : make debug for on...\n";
                for (int i = 0; msg[i]; i++)
                    screen_putchar(msg[i], scr.current);
            #endif
            continue;
        }

        if (key == KEY_TAB) {
            if (scr.mode == SCR_MODE_SPLIT) {
                int other = scr.split_right == scr.current ? scr.split_left : scr.split_right;
                if (!(scr.screens[other].flags & SCR_DEBUG)) {
                    scr.current = other;
                    update_cursor();
                }
            }
            else
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
        if (c == '\b')
            screen_backspace();
        else
            screen_putchar(c, scr.current);
    }
}
