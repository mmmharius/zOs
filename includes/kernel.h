#ifndef KERNEL_H
#define KERNEL_H

#include <keyboard.h>
#include <screen.h>
#include <printk.h>
#include <gdt.h>
#ifdef DEBUG
    #include <debug.h>
#endif

void    zos(void);
void    kshell_run(void);

#endif
