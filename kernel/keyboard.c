#include <keyboard.h>
#include <screen.h>
#include <stdint.h>
#include <io.h>
#include <printk.h>
#ifdef DEBUG
    #include <debug.h>
#endif

int ctrl_pressed = 0;
int shift_pressed = 0;
unsigned char read_keyboard(void) {
    while ((inb(KB_STATUS) & 1) == 0);
    return inb(KB_DATA);
}

char scancode_to_ascii(unsigned char sc) {
    int s = shift_pressed;
    switch (sc) {

        case KEY_1:         return s ? '!' : '1';
        case KEY_2:         return s ? '@' : '2';
        case KEY_3:         return s ? '#' : '3';
        case KEY_4:         return s ? '$' : '4';
        case KEY_5:         return s ? '%' : '5';
        case KEY_6:         return s ? '^' : '6';
        case KEY_7:         return s ? '&' : '7';
        case KEY_8:         return s ? '*' : '8';
        case KEY_9:         return s ? '(' : '9';
        case KEY_0:         return s ? ')' : '0';

        case KEY_Q:         return s ? 'Q' : 'q';
        case KEY_W:         return s ? 'W' : 'w';
        case KEY_E:         return s ? 'E' : 'e';
        case KEY_R:         return s ? 'R' : 'r';
        case KEY_T:         return s ? 'T' : 't';
        case KEY_Y:         return s ? 'Y' : 'y';
        case KEY_U:         return s ? 'U' : 'u';
        case KEY_I:         return s ? 'I' : 'i';
        case KEY_O:         return s ? 'O' : 'o';
        case KEY_P:         return s ? 'P' : 'p';
        case KEY_A:         return s ? 'A' : 'a';
        case KEY_S:         return s ? 'S' : 's';
        case KEY_D:         return s ? 'D' : 'd';
        case KEY_F:         return s ? 'F' : 'f';
        case KEY_G:         return s ? 'G' : 'g';
        case KEY_H:         return s ? 'H' : 'h';
        case KEY_J:         return s ? 'J' : 'j';
        case KEY_K:         return s ? 'K' : 'k';
        case KEY_L:         return s ? 'L' : 'l';
        case KEY_Z:         return s ? 'Z' : 'z';
        case KEY_X:         return s ? 'X' : 'x';
        case KEY_C:         return s ? 'C' : 'c';
        case KEY_V:         return s ? 'V' : 'v';
        case KEY_B:         return s ? 'B' : 'b';
        case KEY_N:         return s ? 'N' : 'n';
        case KEY_M:         return s ? 'M' : 'm';

        case KEY_MINUS:     return s ? '_' : '-';
        case KEY_EQUAL:     return s ? '+' : '=';
        case KEY_LBRACE:    return s ? '{' : '[';
        case KEY_RBRACE:    return s ? '}' : ']';
        case KEY_SEMI:      return s ? ':' : ';';
        case KEY_QUOTE:     return s ? '"' : '\'';
        case KEY_BSLASH:    return s ? '|' : '\\';
        case KEY_COMMA:     return s ? '<' : ',';
        case KEY_DOT:       return s ? '>' : '.';
        case KEY_SLASH:     return s ? '?' : '/';
        case KEY_GRAVE:     return s ? '~' : '`';
        case KEY_ENTER:     return '\n';
        case KEY_BACKSPACE: return '\b';
        case KEY_SPACE:     return ' ';

        default:            return 0;
    }
}

void keyboard_loop(void (*handler)(char)) {
    while (1) {
        unsigned char sc  = read_keyboard();
        unsigned char key = sc & 0x7F;

        if (key == KEY_LSHIFT || key == KEY_RSHIFT) {
            shift_pressed = !(sc & KEY_RELEASE);
            continue;
        }

        if (key == KEY_CTRL) {
            ctrl_pressed = !(sc & KEY_RELEASE);
            continue;
        }

        if (sc & KEY_RELEASE)
            continue;

        if (ctrl_pressed && key == KEY_G) {
            #ifdef DEBUG
                screen_toggle_debug_split();
                if (scr.mode == SCR_MODE_SPLIT)
                    debug_print_state(sc);
            #else
                printk(1, "debug mode not active : make debug\n");
            #endif
            continue;
        }

        if (key == KEY_TAB) {
            #ifdef DEBUG
                if (scr.mode == SCR_MODE_SPLIT &&
                    (scr.screens[scr.split_r].flags & SCR_DEBUG)) {
                    screen_toggle_debug_split();
                    continue;
                }
            #endif
            if (scr.mode == SCR_MODE_SPLIT) {
                int other = (scr.current == scr.split_l) ? scr.split_r : scr.split_l;

                if (!(scr.screens[other].flags & SCR_DEBUG)) {
                    scr.current = other;
                    update_cursor();
                }
            } else {
                int next = (scr.current + 1) % MAX_SCREENS;
                while (scr.screens[next].flags & SCR_DEBUG)
                    next = (next + 1) % MAX_SCREENS;
                screen_switch(next);
            }
            continue;
        }

        if (ctrl_pressed && key == KEY_1) {
            screen_toggle_split();
            continue;
        }

        char c = scancode_to_ascii(sc);
        if (c == 0)
            continue;
        if (handler)
            handler(c);
        #ifdef DEBUG
            if (scr.mode == SCR_MODE_SPLIT && (scr.screens[scr.split_r].flags & SCR_DEBUG))
                debug_print_state(sc);
        #endif
    }
}