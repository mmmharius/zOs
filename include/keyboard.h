#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KB_DATA 0x60
#define KB_STATUS 0x64
#define KEY_ENTER 0x1C
#define KEY_BACKSPACE 0x0E
#define KEY_Q 0x10
#define KEY_W 0x11
#define KEY_E 0x12
#define KEY_R 0x13
#define KEY_T 0x14
#define KEY_Y 0x15
#define KEY_U 0x16
#define KEY_I 0x17
#define KEY_O 0x18
#define KEY_P 0x19
#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20
#define KEY_F 0x21
#define KEY_G 0x22
#define KEY_H 0x23
#define KEY_J 0x24
#define KEY_K 0x25
#define KEY_L 0x26
#define KEY_Z 0x2C
#define KEY_X 0x2D
#define KEY_C 0x2E
#define KEY_V 0x2F
#define KEY_B 0x30
#define KEY_N 0x31
#define KEY_M 0x32

unsigned char   read_keyboard();
char            scancode_to_ascii(unsigned char sc);
void            handle_backspace();
void            print_keyboard(char c);
void            print_keyboard(char c);
void            keyboard_loop();

#endif