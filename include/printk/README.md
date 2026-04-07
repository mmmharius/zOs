# include/printk/ — formatted output library

`printk` is a `printf`-style output function for the kernel.
It supports writing to VGA (screen), serial port (COM1), or the debug screen.

Builds into `printk.a`, linked into every kernel binary.

---

## Files

```
includes/
  printk.h       declarations + output destination constants
src/
  printk.c       main printk(), format dispatch, character/string output
  ft_puthex.c    %x / %X  — hex formatting
  ft_putnbr.c    %d / %i  — signed decimal
  ft_putnsigned.c  %u     — unsigned decimal
  ft_putptr.c    %p       — pointer (0x... format)
```

---

## Output destinations

```c
// includes/printk.h
#define VGA    0    // write to the currently active screen (scr.current)
#define SERIAL 1    // write to COM1 serial port (0x3F8)
#define DBG    2    // write to screen 2 (debug screen), DEBUG build only
```

Usage:
```c
printk(VGA,    "hello %s\n", name);     // → active screen
printk(SERIAL, "error: %d\n", code);   // → serial console (QEMU stdio)
printk(DBG,    "[state] row=%d\n", r); // → debug screen (DEBUG only)
```

---

## How printk works

```c
int printk(int output, const char *str, ...) {
    uint32_t *args = (uint32_t *)(&str + 1);   // args start right after str on stack

    for (int i = 0; str[i]; i++) {
        if (str[i] == '%') {
            ft_formats(args, str[i + 1], output);
            args++;
            i++;
        } else {
            ft_kputchar(str[i], output);
        }
    }
}
```

There is no `va_list` — arguments are read directly from the stack using
a pointer cast. This works because the kernel is compiled as 32-bit cdecl
(`-m32`), where all arguments are pushed onto the stack in order.
In 32-bit cdecl, `&str + 1` points exactly to the first variadic argument.

This would not work in 64-bit code (where args go in registers first).

### Format specifiers

```
%c   single character                  ft_kputchar
%s   null-terminated string            ft_putstr
%d   signed decimal integer            ft_putnbr
%i   same as %d
%u   unsigned decimal integer          ft_putnsigned
%x   hex lowercase                     ft_puthex
%X   hex uppercase                     ft_puthex
%p   pointer  (0x + 8 hex digits)      ft_putptr
%%   literal percent sign
```

---

## Output routing — ft_kputchar

```c
int ft_kputchar(uint8_t c, int output) {
    if (output == SERIAL) {
        while ((inb(COM1 + 5) & 0x20) == 0);  // wait: transmitter empty
        outb(COM1, c);
        return 1;
    }
    #ifdef DEBUG
    if (output == DBG) {
        screen_putchar(c, DEBUG_SCREEN_ID);    // screen 2
        return 1;
    }
    #endif
    screen_putchar(c, scr.current);            // active screen
    return 1;
}
```

### Why the serial wait loop ?

`COM1 + 5 = 0x3FD` is the Line Status Register of the UART.
Bit 5 (0x20) = "transmitter holding register empty" = safe to send a byte.

If we write before the previous byte is sent, it gets silently dropped.

```
COM1 serial port registers (base = 0x3F8):

  +0  Data Register          read/write a byte
  +1  Interrupt Enable Reg   (we don't use interrupts)
  +2  Interrupt ID Reg
  +3  Line Control Reg
  +4  Modem Control Reg
  +5  Line Status Reg        bit 5 = TX empty ← we check this
  +6  Modem Status Reg
  +7  Scratch Reg
```

---

## Build

```sh
# from include/printk/
make          # builds printk.a
make clean
make fclean
make re

# debug build (enables DBG output path):
make EXTRA_CFLAGS=-DDEBUG
```

The main `Makefile` handles this automatically — `make debug` passes
`EXTRA_CFLAGS=-DDEBUG` when building the printk sub-library.

