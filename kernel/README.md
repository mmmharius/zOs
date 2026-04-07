# kernel/ — main entry point, keyboard input, debug system

Files in this folder:
- `kernel.c` — `main()`, the C entry point
- `keyboard.c` — PS/2 input polling, scancode to ASCII, key loop
- `debug.c` — debug state printing, key history (DEBUG build only)
- `screen/` — see `screen/README.md`

---

## kernel.c — main()

`main()` is called from `boot/boot.asm` right after `cli`.
It does three things and never returns:

```c
int main() {
    screen_init();        // set up 4 virtual screens

    printk(VGA, logo);   // print the zOS ASCII art to screen 0

    keyboard_loop();      // enter infinite input loop
    while (1);            // (never reached, satisfies compiler)
}
```

The logo printed at boot depends on the build mode:

```
make       → 5-line "ZOS" ASCII art
make debug → same art with "DEBUG VERSION" prefix, plus serial dump of all screens
make corr  → just prints "42"
```

---

## keyboard.c — PS/2 keyboard input

### How the hardware works

A PS/2 keyboard sends 1-byte "scancodes" to the CPU through two I/O ports:

```
0x60  KB_DATA    → read the scancode byte
0x64  KB_STATUS  → check if data is ready (bit 0 must be 1 before reading)
```

`inb()` / `outb()` in `include/io.h` wrap the x86 `in`/`out` instructions
for direct port access.

### Why polling and not interrupts

A normal OS uses IRQ1 (hardware interrupt) to be notified when a key is
pressed. This kernel does not set up an IDT (interrupt descriptor table),
so interrupts are left off (`cli` in boot.asm) permanently.

Instead, `read_keyboard()` spins in a loop until data is ready:

```c
unsigned char read_keyboard() {
    while ((inb(KB_STATUS) & 1) == 0);   // wait for bit 0
    return inb(KB_DATA);
}
```

The CPU burns 100% while waiting for input. Acceptable for a small kernel.

### Scancodes — make and break codes

A scancode is NOT an ASCII value. It's a raw hardware code.
The PS/2 Set 1 standard defines two events per key:

```
Make  code = key pressed   → bit 7 = 0  (e.g. 'A' press   = 0x1E)
Break code = key released  → bit 7 = 1  (e.g. 'A' release = 0x9E)

0x1E        = 0001 1110  → bit 7 = 0  (make)
0x9E        = 1001 1110  → bit 7 = 1  (break)

0x1E | 0x80 = 0x9E   same key but release
0x9E & 0x7F = 0x1E   strip bit 7 to get key id
```

In `keyboard_loop()`:
```c
unsigned char sc  = read_keyboard();
unsigned char key = sc & 0x7F;           // strip bit 7

if (sc & KEY_RELEASE)   // 0x80
    continue;           // ignore key-up, except Ctrl (see below)
```

### Scancode table

```
Row 1:  0x02='1'

Row 2 (QWERTY):
  0x0F=TAB  0x10=Q  0x11=W  0x12=E  0x13=R  0x14=T
  0x15=Y    0x16=U  0x17=I  0x18=O  0x19=P

Row 3 (ASDF):
  0x1C=ENTER  0x1D=CTRL  0x1E=A  0x1F=S  0x20=D  0x21=F
  0x22=G      0x23=H     0x24=J  0x25=K  0x26=L

Row 4 (ZXCV):
  0x2C=Z  0x2D=X  0x2E=C  0x2F=V  0x30=B  0x31=N  0x32=M

Other:
  0x0E=BACKSPACE  0x39=SPACE  0x80=KEY_RELEASE (bit flag)
```

Why are the rows not contiguous? This is how the PS/2 hardware standard was
designed — gaps correspond to keys like Caps Lock, Shift, etc. that sit
between rows on the physical keyboard.

### Ctrl handling

Ctrl is a modifier key. We need to track both its press AND release:

```c
int ctrl_pressed = 0;

if (key == KEY_CTRL) {
    ctrl_pressed = !(sc & KEY_RELEASE);
    //              └── release event? ctrl_pressed = 0
    //                  press  event? ctrl_pressed = 1
    continue;
}
```

For all other keys, we skip release events entirely (the `continue` above
the Ctrl block). But for Ctrl we must handle release to reset the flag,
otherwise Ctrl would stay "pressed" forever after you release it.

### Ctrl+G — toggle debug screen

```c
if (ctrl_pressed && key == KEY_G) {   // KEY_G = 0x22
    #ifdef DEBUG
        toggle_debug_screen();
    #else
        // print a message explaining debug mode is not active
    #endif
    continue;
}
```

### Tab — switch screens

```
Normal mode: screen_switch((scr.current + 1) % MAX_SCREENS)
  → cycles 0 → 1 → 2 → 3 → 0 → ...

Split mode:
  int other = (scr.split_right == scr.current) ? scr.split_left : scr.split_right;
  if the other panel is NOT the debug screen:
    scr.current = other
    update_cursor()
```

In split mode, Tab only moves cursor focus between panels. It never makes
the debug screen (screen 2 with SCR_DEBUG) the active typing target.

### Key 1 — toggle split view

```c
if (key == KEY_1)
    screen_toggle_split();
```

See `screen/README.md` for split mode details.

### Full input flow

```
keyboard_loop() — infinite:

  read_keyboard()
    └─ spin on KB_STATUS bit 0
    └─ read KB_DATA → raw scancode sc

  key = sc & 0x7F      strip release bit

  ┌─── key == CTRL ────────────────────────────────────────────────┐
  │    ctrl_pressed = !(sc & 0x80)                                 │
  │    continue                                                    │
  └────────────────────────────────────────────────────────────────┘

  ┌─── sc & KEY_RELEASE ───────────────────────────────────────────┐
  │    continue  (ignore key-up for all non-Ctrl keys)             │
  └────────────────────────────────────────────────────────────────┘

  ┌─── ctrl_pressed && key == KEY_G ───────────────────────────────┐
  │    toggle_debug_screen()  (DEBUG only)                         │
  │    continue                                                    │
  └────────────────────────────────────────────────────────────────┘

  ┌─── key == KEY_TAB ─────────────────────────────────────────────┐
  │    switch screen or move split focus                           │
  │    continue                                                    │
  └────────────────────────────────────────────────────────────────┘

  ┌─── key == KEY_1 ───────────────────────────────────────────────┐
  │    screen_toggle_split()                                       │
  │    continue                                                    │
  └────────────────────────────────────────────────────────────────┘

  c = scancode_to_ascii(sc)    convert to ASCII char (or 0 if unknown)
  debug_print_state(sc)        update debug panel (DEBUG only)

  if c == 0:    skip (unknown key)
  if c == '\b': screen_backspace()
  else:         screen_putchar(c, scr.current)
```

---

## debug.c — debug state display (DEBUG build only)

`debug_print_state(sc)` is called on every keypress.
It rewrites the top 4 rows of screen 2 with the current kernel state
(last key, screen ids, cursor position, flags), followed by a rolling
history of the last `DBG_HISTORY_SIZE = 10` keypresses.

For the visual layout of the debug panel, see `kernel/screen/README.md`.

`print_all_screen()` (called at DEBUG startup) dumps all 4 screen buffers
to the serial port, one row per line — visible in QEMU with `-serial stdio`.
