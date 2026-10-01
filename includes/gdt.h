#ifndef GDT_H
#define GDT_H

#define GDT_ADDR    0x800
#define GDT_ENTRIES 7

typedef struct  gdt_entry {
    uint16_t    limit_low;
    uint16_t    base_low;
    uint8_t     base_mid;
    uint8_t     access;
    uint8_t     flags_limit_high; 
    uint8_t     base_high; 
} __attribute__((packed)) gdt_entry;

typedef struct  gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr;

_Static_assert(sizeof(struct gdt_entry) == 8, "gdt_entry must be 8 bytes");
_Static_assert(sizeof(struct gdt_ptr) == 6, "gdt_entry must be 6 bytes");

void    gdt_init();
void    gdt_flush(struct gdt_ptr *p);

#endif