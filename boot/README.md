# boot/ — Multiboot header and kernel entry point

This folder has one file: `boot.asm`.
It is the very first code that runs when GRUB hands control to the kernel.

---

## What GRUB needs

GRUB is the bootloader. It loads the kernel binary from disk into RAM,
but first it needs to verify it is actually looking at a valid kernel.
It does that by scanning the first 8192 bytes of the binary for a
specific magic number: `0x1BADB002`.

This signature, plus two other values, form the **Multiboot header**.
It must appear in a dedicated section (`.multiboot`) so the linker
places it at the very start of the binary.

---

## The Multiboot header

```asm
MBOOT_MAGIC     equ 0x1BADB002
MBOOT_FLAGS     equ 0x00000003
MBOOT_CHECKSUM  equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
align 4
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM
```

### MBOOT_MAGIC = 0x1BADB002

Just an agreed-upon constant from the Multiboot spec.
GRUB looks for this exact value. If it finds it, it knows this is a
valid Multiboot kernel.

### MBOOT_FLAGS = 0x00000003

FLAGS is a bit field. Each bit enables a feature GRUB should provide:

```
0x00000003 in binary:

  0000 0000 0000 0000 0000 0000 0000 0011
                                        ││
                                        │└─ bit 0 (0x01): align modules to 4 KB pages
                                        └── bit 1 (0x02): ask GRUB for a memory map

  0x01 + 0x02 = 0x03
```

We ask for the memory map so the kernel can know the total amount of RAM
(not used yet, but useful later). Module alignment is good practice for
future paging support.

### MBOOT_CHECKSUM = -(MBOOT_MAGIC + MBOOT_FLAGS)

The Multiboot spec requires that these three values sum to zero (mod 2^32):

```
MAGIC + FLAGS + CHECKSUM = 0

so: CHECKSUM = -(MAGIC + FLAGS)
             = -(0x1BADB002 + 0x00000003)
             = -(0x1BADB005)
             =  0xE4524FFB   (32-bit two's complement)

verify: 0x1BADB005 + 0xE4524FFB = 0x100000000
                                = 0x00000000  (lower 32 bits, overflow ignored)
```

This lets GRUB verify it found a real header and not random bytes
that happen to match the magic number.

---

## _start — the entry point

```asm
section .text
global _start
extern main       ; defined in kernel/kernel.c

_start:
    cli           ; disable hardware interrupts
    call main     ; jump to C code
    hlt           ; halt CPU if main() ever returns
    jmp _start    ; safety loop
```

### Why cli ?

`cli` = Clear Interrupt Flag. It tells the CPU to ignore hardware interrupts.

When a key is pressed, a timer fires, or any hardware event happens,
the CPU normally jumps to an "interrupt handler" function.
We haven't set up any interrupt handlers, so if one fired here the CPU
would jump to a garbage address and crash.

We keep interrupts off for the entire lifetime of this kernel — input is
handled by manual polling instead (see `kernel/README.md`).

### Why hlt after call main ?

`main()` never returns in normal operation (it ends in `while(1)`).
But if it ever did, `hlt` stops the CPU from executing whatever random
instructions happen to sit after `_start` in memory.
`hlt` pauses the CPU until the next interrupt — which can't arrive
because we ran `cli`. Then `jmp _start` loops back just in case.

---

## Why .multiboot is a separate section

The linker script (`linker.ld`) places sections in this order:

```
.multiboot   ← first, at 0x100000
.text        ← code
.rodata      ← constant strings
.data        ← initialized globals
.bss         ← zero-initialized globals
```

If the multiboot header was mixed into `.text`, the linker might place
other functions before it and push the header past the 8192-byte limit
that GRUB scans. A dedicated section guarantees it is always first.

---

## Why the kernel loads at 1 MB (0x100000)

Everything below 1 MB is reserved for BIOS, GRUB, and hardware-mapped regions.
Loading a kernel there would overwrite them.

```
Physical memory layout:

  0x00000000 – 0x000003FF   IVT — real mode interrupt vector table
  0x00000400 – 0x000004FF   BIOS data area
  0x00007C00 – 0x00007DFF   MBR / stage 1 bootloader
  0x000A0000 – 0x000BFFFF   VGA memory (text + graphics)
  0x000C0000 – 0x000FFFFF   BIOS ROM, option ROMs — not usable

  0x00100000  ← first safe address → kernel loads here
```

The gap 0xA0000–0xFFFFF is the ISA hole: hardware-mapped, not real RAM.

The linker script places sections in this order at 0x100000:

```
.multiboot   MBOOT_MAGIC, FLAGS, CHECKSUM  (must be first, GRUB scans here)
.text        all code
.rodata      string literals, constants
.data        initialized globals
.bss         zero-initialized globals (screen_mgr_t scr lives here)
```

`.bss` is not stored in the binary file — the linker just records its size.
GRUB zeroes that memory when loading the ELF. That is why `scr` starts
fully zeroed before `screen_init()` runs.
