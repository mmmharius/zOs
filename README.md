# zOS — bare-metal x86 kernel

Minimal 32-bit x86 OS written in C and NASM assembly.
Boots with GRUB, writes directly to VGA memory, reads PS/2 keyboard
by polling. No libc, no syscalls, no filesystem. Everything runs in
ring 0 and talks to hardware through I/O ports and memory-mapped registers.

---

## Clone and setup

```sh
git clone git@github.com:mmmharius/zOs.git
cd zOs

# libasm_zOs is a separate repo used as a sub-library
# if the include/libasm_zOs/ folder is empty after cloning:
git clone git@github.com:mmmharius/libasm_zOs.git include/libasm_zOs
```

---

## Dependencies

```sh
# Ubuntu / Debian
sudo apt install nasm gcc grub-pc-bin grub-common xorriso qemu-system-x86

# check versions used in development:
nasm --version          # >= 2.14
gcc --version           # >= 9  (needs -m32 / multilib)
qemu-system-i386 --version
grub-mkrescue --version
```

If `gcc -m32` fails, install 32-bit multilib support:

```sh
sudo apt install gcc-multilib
```

---

## Build targets

```sh
make          # build kernel.bin — normal mode
make run      # build + ISO + launch in QEMU (shortcut for everything)
make debug    # build with -DDEBUG → enables Ctrl+G debug panel
make corr     # build with -DCORR  → just prints "42" (correction mode)
make iso      # only create the ISO, no QEMU launch
make clean    # remove object files
make fclean   # remove everything (objects + kernel.bin + ISO)
make re       # fclean + make
```

QEMU is launched with `-serial stdio` so serial output (`printk SERIAL`)
appears directly in the terminal.

---

## Key bindings

| Key         | Action                                           |
|-------------|--------------------------------------------------|
| `Tab`       | Switch to next screen (normal mode)              |
| `Tab`       | Move cursor focus between panels (split mode)    |
| `1`         | Toggle split view on/off                         |
| `Ctrl+G`    | Toggle debug panel (requires `make debug`)       |
| `Backspace` | Delete last character                            |
| `Enter`     | New line                                         |

---

## Folder map

```
zOs/
│
├── boot/                   GRUB multiboot header + _start entry point
│   ├── boot.asm            first code that runs, calls main()
│   └── README.md           ← multiboot header, cli, linker script, 1MB
│
├── kernel/                 C kernel code
│   ├── kernel.c            main(): screen_init + logo + keyboard_loop
│   ├── keyboard.c          PS/2 polling, scancodes, keyboard_loop()
│   ├── debug.c             debug state display (DEBUG only)
│   ├── README.md           ← keyboard, scancodes, Ctrl, Tab, debug
│   └── screen/             VGA + virtual screen system
│       ├── screen_core.c   screen_init, screen_switch, toggle_debug
│       ├── screen_display.c  VGA rendering, split refresh
│       ├── screen_io.c     screen_putchar, screen_backspace
│       ├── screen_utils.c  get_vga_pos, update_cursor, scroll
│       └── README.md       ← VGA cells, colors, flags, split, scroll
│
├── include/
│   ├── color.h             VGA_COLOR() macro, 16-color palette
│   ├── debug.h             DBG_HISTORY_SIZE, debug declarations
│   ├── io.h                inb() / outb() inline ASM port wrappers
│   ├── keyboard.h          PS/2 scancode defines (KB_DATA, KEY_*)
│   ├── screen.h            screen_t, screen_mgr_t, SCR_* flags
│   ├── stdint.h            uint8_t, uint16_t, uint32_t…
│   ├── init.h              (unused — dead declaration)
│   │
│   ├── printk/             printf-style output library → printk.a
│   │   ├── includes/printk.h
│   │   ├── src/            printk.c, ft_puthex.c, ft_putnbr.c…
│   │   └── README.md       ← printk(), format specifiers, serial output
│   │
│   └── libasm_zOs/         ASM string/memory utility library → libasm_zOs.a
│       ├── ft_strlen.s
│       ├── ft_strcpy.s
│       ├── ft_strcmp.s
│       ├── libasm.h
│       └── README.md       ← current state, arch issue, future functions
│
├── linker.ld               kernel placed at 1 MB, section order
└── Makefile
```

---

## I/O ports used by the kernel

```
Port    What                    Used in
────────────────────────────────────────────────────────────
0x60    PS/2 keyboard data      keyboard.c — read scancode byte
0x64    PS/2 keyboard status    keyboard.c — bit 0: data ready

0x3D4   VGA CRTC index reg      screen_utils.c — select cursor register
0x3D5   VGA CRTC data  reg      screen_utils.c — write cursor position
          reg 0x0E = cursor high byte
          reg 0x0F = cursor low  byte

0x3F8   COM1 serial data        printk.c — send byte to serial
0x3FD   COM1 line status        printk.c — bit 5: TX empty, safe to write
```

---

## Known issues

**`include/printk/includes/printk.h`**
- Includes `<debug.h>` which includes `<printk.h>` back — circular dependency.
  `printk.h` doesn't need `debug.h`. Remove that include.
- `extern int ROW;` and `extern int COL;` declared but never defined or used.
  Dead code from an earlier version.

**`include/init.h`**
- `void init();` declared but never implemented and never called. Dead file.

**`linker.ld`**
- Comment says `8192bit` — should be `8192 bytes`.

**`include/libasm_zOs/`**
- Compiled as `elf64` but kernel is `elf32` — arch mismatch.
  Functions use 64-bit registers, would crash if called from kernel code.
- `libasm.h` includes `<stddef.h>` and `<sys/types.h>` (Linux system headers).
  Needs to use the kernel's own `stdint.h`.
- None of the functions are called anywhere yet.
- Fix details in `include/libasm_zOs/README.md`.

**`include/color.h`**
- Screen 3 has no color. `get_screen_color()` falls back to screen 0 color —
  screen 3 and screen 0 look identical.
