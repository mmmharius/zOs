#include <keyboard.h>
#include <screen.h>
#include <printk.h>

int SCREEN = 1;
int HALF_SCREEN = 0;

t_screen screens[NB_SCREEN];
t_screen *current;

t_screen s_creen = {
    .row = 0,
    .col = 0,
    .scroll = 0
};

void    init_screen() {
    for (int i = 0; i <= (NB_SCREEN - 1); i++) {
        printk(1, "%d, %d", i, (NB_SCREEN - 1));
        screens[i].row = 0;
        screens[i].col = 0;
        screens[i].scroll = 0;
        screens[i].row_max = VGA_HEIGHT;
        screens[i].col_max = VGA_WIDTH;
        screens[i].row_start = 0;
        screens[i].col_start = 0;
        screens[i].row_half = 0;
        screens[i].col_half = 0;
        for (int j = START_PRINT; j < VGA_WIDTH * (VGA_HEIGHT * SCREEN_LENGHT); j++)
            screens[i].buffer[j] = (uint16_t)' ' | VGA_DEFAULT_COLOR;
    }
    current = &screens[0];
}

void    init() {
    init_screen();
}


void    move_cursor() {
    uint16_t pos = current->row * VGA_WIDTH + current->col;
    
    outb(0x3D4, 0x0F); // 0x0F  = cursor low byte
    // 0x3D4 = choose register | 
    outb(0x3D5, (uint8_t)(pos & 0xFF)); 
    outb(0x3D4, 0x0E);  // 0x0E  = cursor high byte
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void    move_cursor_half() {
    uint16_t pos = current->row_half * VGA_WIDTH + current->col_half;
    
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF)); 
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void    load_screen() {
    volatile uint16_t* vga = (uint16_t*)VGA_ADDR;
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            uint16_t c = current->buffer[(row * VGA_WIDTH) + col];
            vga[col + row * VGA_WIDTH] = (uint16_t)c | VGA_DEFAULT_COLOR;
        }
    }
}
