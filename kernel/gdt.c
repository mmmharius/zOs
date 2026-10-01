#include "kernel.h"

static void gdt_set_entry(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt_entry *gdt = (gdt_entry *)GDT_ADDR;

    gdt[i].base_high = (base >> 24);
    gdt[i].base_mid = (base >> 16) & 0xFF;
    gdt[i].base_low = base & 0xFFFF;
    gdt[i].limit_low = limit & 0xFFFF;
    gdt[i].access = access;
    gdt[i].flags_limit_high = (flags & 0xF0) | ((limit >> 16) & 0x0F);

    #ifdef DEBUG
        printk(1, "%x\r\n", gdt[i].base_high);
        printk(1, "%x\r\n", gdt[i].base_mid);
        printk(1, "%x\r\n", gdt[i].base_low);
        printk(1, "%x\r\n", gdt[i].limit_low);
        printk(1, "%x\r\n", gdt[i].access);
        printk(1, "%x\r\n", gdt[i].flags_limit_high);
        printk(SERIAL, "%p\n", 0x00100800);
    #endif
}

void    gdt_init() {
    gdt_ptr ptr;
    ptr.base = GDT_ADDR;
    ptr.limit = (GDT_ENTRIES * 8) - 1; // *8 bc gdt_entry is 8octet, -1 bc start at 0.
    gdt_set_entry(0, 0, 0xFFFFF, 0, 0);    // null descriptor
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xC0);    // kernel code
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xC0);    // kernel data
    gdt_set_entry(3, 0, 0xFFFFF, 0xFA, 0xC0);    // user code
    gdt_set_entry(4, 0, 0xFFFFF, 0xF2, 0xC0);    // user data
    gdt_flush(&ptr);
}   
