OUTPUT_FORMAT("elf32-i386")     /* output 32-bit x86 ELF binary */
ENTRY(_start)                   /* entry point (defined in boot.asm) */

SECTIONS
{
    . = 1M;                         /* kernel load at 1MB (GRUB requirement) */

    .multiboot : { *(.multiboot) }  /* multiboot first (need to be in first 8192bit of kernel) */

    .text : { *(.text) }            /* code section (rx) */

    .rodata : { *(.rodata) }        /* read-only data (r) */

    .data : { *(.data) }            /* init data (rw) */

    .bss : { *(.bss) }              /* uninit data (rw) */
}
