OUTPUT_FORMAT("elf32-i386")     /* output 32-bit x86 ELF binary */
ENTRY(_start)                   /* entry point (defined in boot.asm) */

SECTIONS
{
    . = 1M;                     /* kernel load at 1MB (GRUB requirement) */
    
    .text : {
        *(.multiboot)           /* multiboot first (need to be in first 8mb) */
        *(.text)                /* code section */
    }
    
    .data : {
        *(.data)                /* init data */
    }
    
    .bss : {
        *(.bss)                 /* uninit data */
    }
}