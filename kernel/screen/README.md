# kernel/screen/ — VGA text mode and virtual screen system

Four files:
- `screen_core.c` — init, screen switch, debug screen toggle
- `screen_display.c` — VGA rendering, split refresh, split toggle
- `screen_io.c` — putchar, backspace
- `screen_utils.c` — cursor, VGA position, scroll, row clear

All structures and defines are in `include/screen.h`.

---

## VGA text mode basics

VGA text mode is a hardware feature built into x86 PCs.
The CPU can write characters directly to a memory region starting at
`0xB8000` and they appear on screen immediately — no GPU driver needed.

Each character cell on screen is 2 bytes (a `uint16_t`):

```
vga[0] = (uint16_t)'4' | 0x0F00

(uint16_t)'4' in ASCII  = 0x0034
attribute (white/black) = 0x0F00
result                  = 0x0F34

0x0F34 = 0000 1111 0011 0100
         └┬─┘ └┬─┘ └───┬───┘
          │    │         └─ ASCII character ('4')
          │    └── foreground color of the text (0xF = white)
          └── background color (0x0 = black)

Memory layout (little endian, as stored in RAM):
  [0xB8000] = 0x34   low  byte → ASCII character
  [0xB8001] = 0x0F   high byte → color attribute
```

The screen is 80 columns × 25 rows = 2000 cells = 4000 bytes total.

```
VGA cell index layout:

  col:    0    1    2  ...  79
row  0: [  0][  1][  2]...[  79]
row  1: [ 80][ 81][ 82]...[ 159]
row  2: [160][161][162]...[ 239]
  ...
row 24: [1920][1921]...........[1999]

formula: index = row * 80 + col
```

---

## VGA color encoding

The 16-bit cell value holds both the character and its colors.
Colors are defined in `include/color.h`:

```
VGA 16-color palette:

  0x0  BLACK       0x8  D_GRAY
  0x1  BLUE        0x9  L_BLUE
  0x2  GREEN       0xA  L_GREEN
  0x3  CYAN        0xB  L_CYAN
  0x4  RED         0xC  L_RED
  0x5  MAGENTA     0xD  L_MAGENTA
  0x6  BROWN       0xE  YELLOW
  0x7  L_GRAY      0xF  WHITE
```

The macro `VGA_COLOR(fg, bg)` builds the 16-bit color attribute:

```c
#define VGA_COLOR(fg, bg)  ((bg << 12) | (fg << 8))
```

```
VGA_COLOR(L_BLUE, BLACK) = VGA_COLOR(0x9, 0x0)

  (bg << 12) = 0x0 << 12 = 0x0000
  (fg <<  8) = 0x9 <<  8 = 0x0900

  result = 0x0900

  0x0900 = 0000 1001 0000 0000
           └┬─┘ └┬─┘ └───┬───┘
            │    │         └─ ASCII byte (0x00, OR'd with char at write time)
            │    └── foreground color: 1001 = 0x9 = L_BLUE
            └── background color: 0000 = 0x0 = BLACK
```

Each screen has its own fixed text color:

```
screen 0 → VGA_COLOR(L_BLUE, BLACK)  → light blue text
screen 1 → VGA_COLOR(L_RED,  BLACK)  → light red text
screen 2 → VGA_COLOR(WHITE,  BLACK)  → white text  (debug screen)
screen 3 → fallback to screen 0 color (no unique color defined)
```

---

## Virtual screens

The kernel keeps 4 independent screen buffers in RAM.
Only the active one (or two, in split mode) is shown on VGA hardware.

```c
// include/screen.h

typedef struct {
    int     row;                             // cursor row   (0–24)
    int     col;                             // cursor col   (0–79)
    int     start_row;                       // first writable row
    uint8_t flags;                           // control bits (see below)
    char    buffer[VGA_WIDTH * VGA_HEIGHT];  // 2000 chars, no color stored here
} screen_t;

typedef struct {
    screen_t screens[MAX_SCREENS];  // 4 screens
    int      count;                 // always 4
    int      current;               // active screen index (0–3)
    int      mode;                  // SCR_MODE_NORMAL or SCR_MODE_SPLIT
    int      split_left;            // left panel id in split mode
    int      split_right;           // right panel id in split mode
} screen_mgr_t;

extern screen_mgr_t scr;  // one global, lives in .bss (zeroed by GRUB at load)
```

The `buffer[]` stores only ASCII characters. Color is applied at render time
by OR-ing the char with the color attribute before writing to VGA memory.

Why a buffer in RAM instead of writing directly to VGA?
- Screen switching is instant: just re-render a different buffer to VGA
- Invisible screens can still receive characters (useful for the debug screen)
- Scrolling works in RAM first, then one VGA update at the end

---

## Screen flags

```c
#define SCR_ACTIVE   (1 << 0)   // 0x01
#define SCR_RENDERED (1 << 1)   // 0x02
#define SCR_DEBUG    (1 << 2)   // 0x04
#define SCR_SPLIT_L  (1 << 3)   // 0x08
#define SCR_SPLIT_R  (1 << 4)   // 0x10
```

Packed into one `uint8_t` per screen:

```
flags = 0 0 0 R L D E A
              │ │ │ │ └─ bit 0: ACTIVE    screen is initialized
              │ │ │ └── bit 1: RENDERED   content is shown on VGA right now
              │ │ └─── bit 2: DEBUG      this is the debug screen
              │ └──── bit 3: SPLIT_L    shown as left half in split mode
              └───── bit 4: SPLIT_R    shown as right half in split mode
```

`SCR_RENDERED` is the key flag. In `screen_putchar()`, a character is written
to VGA hardware only if the screen has this flag set. Invisible screens still
get written to their RAM buffer.

```
After screen_init():

  screen 0: flags = 0x03  (ACTIVE | RENDERED)  ← visible
  screen 1: flags = 0x01  (ACTIVE only)         ← buffered, not shown
  screen 2: flags = 0x01  (ACTIVE only)         ← buffered, not shown
  screen 3: flags = 0x01  (ACTIVE only)         ← buffered, not shown
```

---

## start_row — protected header zone

`start_row` is the first row the cursor can write to. Everything above it
is protected from scrolling.

```
screen 0:  start_row = 5
  rows 0–4 → zOS logo printed at boot, never scrolled away
  rows 5–24 → normal writable zone

screen 1:  start_row = 0
  rows 0–24 → fully writable (no logo)

screen 2:  start_row = 4
  rows 0–3 → debug state: key, screen id, cursor pos, flags
  rows 4–13 → key history (up to DBG_HISTORY_SIZE = 10 entries)

screen 3:  start_row = 0
  rows 0–24 → fully writable
```

Set in `screen_init()`:
```c
scr.screens[i].start_row = (i == 0) ? 5 : (i == DEBUG_SCREEN_ID) ? 4 : 0;
```

---

## Normal mode vs split mode

```
SCR_MODE_NORMAL — one screen fills all 80 columns:

  ┌────────────────────────────────────────────────────────────────────────────┐
  │  screen [scr.current]                                          80 cols     │
  └────────────────────────────────────────────────────────────────────────────┘

SCR_MODE_SPLIT — two screens side by side, 40 columns each:

  ┌──────────────────────────────────┬──────────────────────────────────┐
  │  screen [scr.split_left]         │  screen [scr.split_right]        │
  │  40 cols                         │  40 cols                         │
  └──────────────────────────────────┴──────────────────────────────────┘
  col 0                             col 40                            col 79
```

Toggle with key `1`. On toggle ON:
```c
scr.split_left  = scr.current;
scr.split_right = (scr.current + 1) % MAX_SCREENS;
```

On toggle OFF: split flags are cleared on both screens, `scr.current`
gets `SCR_RENDERED` back, full-width refresh.

---

## VGA position calculation

`get_vga_pos(id, row, col)` converts screen coordinates to a VGA buffer index:

```c
int get_vga_pos(int id, int row, int col) {
    if (scr.mode == SCR_MODE_SPLIT) {
        int offset = (id == scr.split_right) ? VGA_WIDTH / 2 : 0;
        return row * VGA_WIDTH + col + offset;
    }
    return row * VGA_WIDTH + col;
}
```

```
Normal mode — row 2, col 5:
  index = 2 * 80 + 5 = 165

Split mode, left screen (id == split_left) — row 2, col 5:
  offset = 0
  index  = 2 * 80 + 5 + 0 = 165  → VGA column 5

Split mode, right screen (id == split_right) — row 2, col 5:
  offset = 40
  index  = 2 * 80 + 5 + 40 = 205 → VGA column 45
```

The right panel's characters are always shifted 40 columns to the right
in VGA memory, which is how they appear on the right half of the screen.

The screen buffer (`buffer[]`) is always indexed with 80-col math
(`row * VGA_WIDTH + col`), regardless of split mode. Only the VGA write
adds the offset.

---

## screen_putchar — rendering flow

```c
void screen_putchar(char c, int id)
```

```
if c == '\n':
  col = 0
  row++
  (nothing written to buffer, nothing written to VGA)

else:
  buffer[row * 80 + col] = c             ← always write to RAM buffer

  if SCR_RENDERED:
    pos = get_vga_pos(id, row, col)
    vga[pos] = (uint16_t)c | color       ← write to VGA hardware

  col++
  if col >= width: col = 0, row++

if row >= 25:
  scroll(id)
  full refresh (screen_refresh or split_refresh)

update_cursor()
```

`\n` only moves the cursor — it does NOT store anything in the buffer.
Blank cells in the buffer are spaces (filled at init by `screen_init()`).

---

## Scrolling

When `row` reaches 25, `scroll(id)` shifts every row from `start_row`
upward by one:

```
Before (screen 0, start_row = 5, screen is full):

  row  0: [logo 1  ]   ← protected
  row  1: [logo 2  ]   ← protected
  row  2: [logo 3  ]   ← protected
  row  3: [logo 4  ]   ← protected
  row  4: [logo 5  ]   ← protected
  row  5: [line A  ]   ← start_row, will be overwritten
  row  6: [line B  ]
  ...
  row 24: [line T  ]   ← cursor here, needs row 25

After scroll:

  row  0: [logo 1  ]   ← unchanged
  row  1: [logo 2  ]   ← unchanged
  row  2: [logo 3  ]   ← unchanged
  row  3: [logo 4  ]   ← unchanged
  row  4: [logo 5  ]   ← unchanged
  row  5: [line B  ]   ← was row 6 (line A is gone)
  row  6: [line C  ]
  ...
  row 24: [       ]    ← cleared, cursor placed here
```

---

## Cursor control

The VGA CRTC chip controls the blinking cursor position via I/O ports:

```
Port 0x3D4 = index register (select which CRTC register to access)
Port 0x3D5 = data  register (read/write the selected CRTC register)

CRTC register 0x0E = cursor position, high byte
CRTC register 0x0F = cursor position, low  byte

To place cursor at VGA index 165 (= row 2, col 5 = 0x00A5):

  outb(0x3D4, 0x0F)        select low  byte register
  outb(0x3D5, 0xA5)        write low  byte (165 & 0xFF)
  outb(0x3D4, 0x0E)        select high byte register
  outb(0x3D5, 0x00)        write high byte (165 >> 8)
```

The cursor always follows `scr.current`. In split mode, Tab switches
`scr.current` between the two panels and `update_cursor()` moves the
blinking cursor to the new position.

---

## Debug screen (DEBUG build only)

`Ctrl+G` toggles a split view where the right panel is always screen 2
(the debug screen). It gets the `SCR_DEBUG` flag.

```
┌──────────────────────────────────┬──────────────────────────────────┐
│  screen [scr.current]            │  screen 2  (debug)               │
│                                  │ [KEY PRESS]| key=A sc=0x1e       │
│  normal output                   │ [SCREN]    | cur=0 mode=1 ...    │
│                                  │ [CURRENT]  | row=7 col=3         │
│                                  │ [FLAG]     | 0x0B 0x01 0x12      │
│                                  │ [HISTORY]  | A B C D ...         │
└──────────────────────────────────┴──────────────────────────────────┘
```

Tab in this mode will not switch focus to the debug screen
(`SCR_DEBUG` check in `keyboard.c` blocks it).
