# zOS

Kernel x86 32-bit bare-metal. C + NASM. Boot GRUB, VGA text mode, keyboard PS/2.
no libc, no syscalls, no filesystem.

---

## Setup

```sh
git clone git@github.com:mmmharius/zOs.git
cd zOs

# if include/libasm_zOs/ is empty :
git clone git@github.com:mmmharius/libasm_zOs.git include/libasm_zOs
```

### Dependencies

```sh
sudo apt install nasm gcc grub-pc-bin grub-common xorriso qemu-system-x86

# si gcc -m32 fail :
sudo apt install gcc-multilib
```

---

## Build

```sh
make          # kernel.bin
make run      # kernel.bin + ISO + QEMU
make debug    # -DDEBUG → active Ctrl+G debug panel
make corr     # -DCORR  → just displays "42"
make iso      # ISO only, no QEMU
make clean
make fclean
make re
```

QEMU is lunch `-serial stdio` → `printk(SERIAL, ...)` output to the terminal.

---

## Controls

| key         | Action                                              |
|-------------|-----------------------------------------------------|
| `Tab`       | Next Screen (normal mode)                           |
| `Tab`       | Switch to split mode/normal mode                    |
| `1`         | Toggle split view                                   |
| `Ctrl+G`    | Toggle debug panel (`make debug` need)              |
| `Backspace` | Like a normal backspace just delete...              |
| `Enter`     | New ligne                                           |

---

## Structure

```
zOs/
├── boot/
│   ├── boot.asm             multiboot header + _start
│   └── README.md
├── kernel/
│   ├── kernel.c             main()
│   ├── keyboard.c           polling PS/2, keyboard_loop()
│   ├── debug.c              debug state (DEBUG only)
│   ├── README.md
│   └── screen/
│       ├── screen_core.c    screen_init, screen_switch
│       ├── screen_display.c vga_draw_screen, split refresh
│       ├── screen_io.c      screen_putchar, screen_backspace
│       ├── screen_utils.c   get_vga_pos, update_cursor, scroll
│       └── README.md
├── include/
│   ├── color.h              VGA_COLOR(), palette, colors per screen
│   ├── debug.h              DBG_HISTORY_SIZE
│   ├── io.h                 inb() / outb()
│   ├── keyboard.h           scancodes PS/2
│   ├── screen.h             screen_t, screen_mgr_t, SCR_* flags
│   ├── stdint.h             uint8_t, uint16_t, uint32_t…
│   ├── printk_zOs/          → printk_zOs.a  (submodule)
│   │   └── README.md
│   └── libasm_zOs/          → libasm_zOs.a  (submodule)
│       └── README.md
├── linker.ld
└── Makefile
```

---

## Ports I/O

```
0x60   PS/2 data              keyboard.c
0x64   PS/2 status (bit 0)    keyboard.c
0x3D4  VGA CRTC index         screen_utils.c — update_cursor()
0x3D5  VGA CRTC data          screen_utils.c — update_cursor()
         reg 0x0E = cursor high byte
         reg 0x0F = cursor low  byte
0x3F8  COM1 serial data       printk.c
0x3FD  COM1 line status       printk.c — bit 5 : TX empty
```
