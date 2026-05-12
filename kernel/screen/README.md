# kernel/screen/

- `screen_core.c` — screen_init, screen_switch, toggle_debug_screen
- `screen_display.c` — vga_draw_screen, screen_refresh, screen_toggle_split
- `screen_io.c` — screen_putchar, screen_backspace
- `screen_utils.c` — get_vga_pos, update_cursor, scroll, replace_row

Structures et defines : `include/screen.h`.

---

## VGA text mode

La mémoire VGA démarre à `0xB8000`. Chaque cellule = 2 bytes (`uint16_t`) :

```
vga[0] = (uint16_t)'4' | 0x0F00

(uint16_t)'4' en ASCII  = 0x0034
attribut (blanc/noir)   = 0x0F00
résultat                = 0x0F34

0x0F34 = 0000 1111 0011 0100
         └┬─┘ └┬─┘ └───┬───┘
          │    │         └─ caractère ASCII ('4')
          │    └── foreground (0xF = blanc)
          └── background (0x0 = noir)

En mémoire (little endian) :
  [0xB8000] = 0x34  ← caractère
  [0xB8001] = 0x0F  ← attribut couleur
```

80 colonnes × 25 lignes = 2000 cellules = 4000 bytes.

```
index = row * 80 + col

  col:    0    1  ...  79
row  0: [  0][  1]...[  79]
row  1: [ 80][ 81]...[ 159]
  ...
row 24: [1920].......[1999]
```

---

## Couleurs

```
0x0 BLACK    0x8 D_GRAY
0x1 BLUE     0x9 L_BLUE
0x2 GREEN    0xA L_GREEN
0x3 CYAN     0xB L_CYAN
0x4 RED      0xC L_RED
0x5 MAGENTA  0xD L_MAGENTA
0x6 BROWN    0xE YELLOW
0x7 L_GRAY   0xF WHITE
```

```c
#define VGA_COLOR(fg, bg)  ((bg << 12) | (fg << 8))
```

```
VGA_COLOR(L_BLUE, BLACK) = VGA_COLOR(0x9, 0x0)

  0x0900 = 0000 1001 0000 0000
           └┬─┘ └┬─┘ └───┬───┘
            │    │         └─ caractère (0x00, OR'd au write)
            │    └── fg : 1001 = 0x9 = L_BLUE
            └── bg : 0000 = 0x0 = BLACK
```

Couleur par screen :

```
screen 0 → L_BLUE / BLACK
screen 1 → L_RED  / BLACK
screen 2 → WHITE  / BLACK  (debug)
screen 3 → L_GREEN/ BLACK
```

---

## Structures

```c
typedef struct {
    int     row;        // curseur ligne  (0–24)
    int     col;        // curseur colonne (0–79)
    int     start_row;  // première ligne écrivable
    uint8_t flags;
    char    buffer[VGA_WIDTH * VGA_HEIGHT];  // 2000 chars, pas de couleur
} screen_t;

typedef struct {
    screen_t screens[MAX_SCREENS];  // 5 screens
    int      count;       // 4
    int      current;     // screen actif (0–3)
    int      mode;        // SCR_MODE_NORMAL ou SCR_MODE_SPLIT
    int      split_l;
    int      split_r;
} screen_mgr_t;

extern screen_mgr_t scr;
```

---

## Flags

```c
#define SCR_ACTIVE   (1 << 0)  // 0x01
#define SCR_RENDERED (1 << 1)  // 0x02
#define SCR_DEBUG    (1 << 2)  // 0x04
#define SCR_SPLIT_L  (1 << 3)  // 0x08
#define SCR_SPLIT_R  (1 << 4)  // 0x10
```

```
flags = 0 0 0 R L D E A
              │ │ │ │ └─ ACTIVE    : initialisé
              │ │ │ └── RENDERED  : contenu visible sur VGA
              │ │ └─── DEBUG     : screen de debug
              │ └──── SPLIT_L   : moitié gauche du split
              └───── SPLIT_R   : moitié droite du split
```

`screen_putchar` écrit sur VGA uniquement si `SCR_RENDERED` est set.

```
Après screen_init() :

  screen 0 : 0x03  (ACTIVE | RENDERED)
  screen 1 : 0x01  (ACTIVE)
  screen 2 : 0x01  (ACTIVE)
  screen 3 : 0x01  (ACTIVE)
```

---

## start_row

Première ligne écrivable. Les lignes au-dessus ne scrollent pas.

```
screen 0 : start_row = 5   → rows 0–4 = logo zOS
screen 1 : start_row = 0   → tout écrivable
screen 2 : start_row = 4   → rows 0–3 = debug state, rows 4–13 = history
screen 3 : start_row = 0
```

---

## Mode normal / split

```
Normal — 80 cols, 1 screen :

  ┌──────────────────────────────────────────────────────────────────────────┐
  │  screen [scr.current]                                        80 cols     │
  └──────────────────────────────────────────────────────────────────────────┘

Split — 40 cols chaque :

  ┌──────────────────────────────────┬──────────────────────────────────┐
  │  screen [split_l]             │  screen [split_r]            │
  │  40 cols                         │  40 cols                         │
  └──────────────────────────────────┴──────────────────────────────────┘
  col 0                             col 40                            col 79
```

Toggle avec `1` :
```c
scr.split_l  = scr.current;
scr.split_r = (scr.current + 1) % MAX_SCREENS;
```

---

## get_vga_pos

```c
int get_vga_pos(int id, int row, int col) {
    if (scr.mode == SCR_MODE_SPLIT) {
        int offset = (id == scr.split_r) ? VGA_WIDTH / 2 : 0;
        return row * VGA_WIDTH + col + offset;
    }
    return row * VGA_WIDTH + col;
}
```

```
Normal, row=2 col=5      → index 165
Split gauche, row=2 col=5 → 165       (col VGA = 5)
Split droit,  row=2 col=5 → 205       (col VGA = 45)
```

Le buffer est toujours indexé en 80 cols. Seul le write VGA applique l'offset.

---

## screen_putchar

```
c == '\n' :
  col = 0, row++
  rien écrit dans le buffer, rien sur VGA

sinon :
  buffer[row * 80 + col] = c            (toujours)
  si SCR_RENDERED :
    vga[get_vga_pos()] = (uint16_t)c | color
  col++
  col >= width → col = 0, row++

row >= 25 → scroll(id) + refresh
update_cursor()
```

---

## Scroll

```
Avant (start_row=5) :           Après :

  row 0 : [logo 1] ← protégé     row 0 : [logo 1] ← protégé
  row 1 : [logo 2] ← protégé     row 1 : [logo 2] ← protégé
  row 2 : [logo 3] ← protégé     row 2 : [logo 3] ← protégé
  row 3 : [logo 4] ← protégé     row 3 : [logo 4] ← protégé
  row 4 : [logo 5] ← protégé     row 4 : [logo 5] ← protégé
  row 5 : [line A] ← start_row   row 5 : [line B]  ← perdu : line A
  row 6 : [line B]                row 6 : [line C]
  ...                             ...
  row 24: [line T]                row 24: [       ] ← curseur ici
```

---

## Curseur

```
Port 0x3D4 = CRTC index  |  Port 0x3D5 = CRTC data

reg 0x0E = cursor position high byte
reg 0x0F = cursor position low  byte

Placer le curseur à l'index 165 (= row 2, col 5 = 0x00A5) :

  outb(0x3D4, 0x0F)  outb(0x3D5, 0xA5)  ← low  byte
  outb(0x3D4, 0x0E)  outb(0x3D5, 0x00)  ← high byte
```

---

## Debug screen (DEBUG only)

`Ctrl+G` ouvre le split avec screen 2 en panel droit (`SCR_DEBUG`).

```
┌──────────────────────────────────┬──────────────────────────────────┐
│  screen [scr.current]            │  screen 2  (debug)               │
│                                  │ [KEY PRESS]| key=A sc=0x1e       │
│                                  │ [SCREN]    | cur=0 mode=1 ...    │
│                                  │ [CURRENT]  | row=7 col=3         │
│                                  │ [FLAG]     | 0x0B 0x01 0x12      │
│                                  │ [HISTORY]  | A B C D ...         │
└──────────────────────────────────┴──────────────────────────────────┘
```

Tab en split mode ne bascule pas sur le debug screen (check `SCR_DEBUG` dans keyboard.c).
