#include <printk.h>
#include <gdt.h>

void stack_dump(int n) {
    uint32_t *stack_ptr = (uint32_t *)get_esp();

    printk(VGA, "stack esp = %p\n", (uint32_t)stack_ptr);
    for (int i = 0; i < n; i++) {
        printk(VGA, "%p : %x\n", (uint32_t)(stack_ptr + i), stack_ptr[i]);
    }
}