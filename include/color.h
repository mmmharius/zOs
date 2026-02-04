#ifndef COLOR_H
#define COLOR_H

#define BLACK           0x0
#define BLUE            0x1
#define GREEN           0x2
#define CYAN            0x3
#define RED             0x4
#define MAGENTA         0x5
#define BROWN           0x6
#define L_GRAY          0x7
#define D_GRAY          0x8
#define L_BLUE          0x9
#define L_GREEN         0xA
#define L_CYAN          0xB
#define L_RED           0xC
#define L_MAGENTA       0xD
#define YELLOW          0xE
#define WHITE           0xF

#define VGA_COLOR(fg, bg) ((bg << 12) | (fg << 8))

#define VGA_DEFAULT_COLOR VGA_COLOR(BLUE, BLACK)

#endif
