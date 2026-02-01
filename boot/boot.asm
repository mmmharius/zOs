BITS 32

; grub verif magic + flags + checksum = 0
MBOOT_MAGIC     equ 0x1BADB002 ; multiboot spec magic number (required by GRUB)
MBOOT_FLAGS     equ 0x00000003
; flags (bit field):
; - bit 0 (0x01) Align modules on page boundaries
; - bit 1 (0x02) Provide memory map information
; - 0x03 = 0x01 + 0x02 (both)
MBOOT_CHECKSUM  equ -(MBOOT_MAGIC + MBOOT_FLAGS) ; calcul to get the res = 0

; multiboot header must be in first 8KB of binary
section .multiboot
align 4                     ; align to 4-byte boundary (multiboot requirement)
    dd MBOOT_MAGIC          ; dd -> write
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM

section .text
global _start               ; entry point visible to linker
extern kernel_main          ; C function defined in kernel.c

_start:
    cli                     ; disable interrupts (no handlers yet)
    call kernel_main        ; jump to C code
    hlt                     ; halt CPU if kernel_main returns