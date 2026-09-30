#include "kernel.h"

void gdt_set_entry(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt_entry *gdt = (gdt_entry *)GDT_ADDR;

    gdt[i].base_high = (base >> 24);
    gdt[i].base_mid = (base >> 16) & 0xFF;
    gdt[i].base_low = base & 0xFFFF;
    gdt[i].limit_low = limit & 0xFFFF;
    gdt[i].access = access;
    gdt[i].flags_limit_high = (flags & 0xF0) | ((limit >> 16) & 0x0F);

    printk(1, "%x\r\n", gdt[i].base_high);
    printk(1, "%x\r\n", gdt[i].base_mid);
    printk(1, "%x\r\n", gdt[i].base_low);
    printk(1, "%x\r\n", gdt[i].limit_low);
    printk(1, "%x\r\n", gdt[i].access);
    printk(1, "%x\r\n", gdt[i].flags_limit_high);
}