/*
 * monitor.c -- Defines functions to control the monitor.
 * From JamesM's kernel development tutorials.
 */

#include <stdarg.h>

#include "monitor.h"

/* The VGA framebuffer starts at 0xB8000 */
static u16int *video_memory = (u16int *) 0xB8000;
/* Stores the cursor position. */
static u8int cursor_x = 0;
static u8int cursor_y = 0;

/*
 * Updates the hardware cursor.
 */
static void move_cursor(void)
{
    /* The screen is 80 characters wide. */
    u16int cursorLocation = cursor_y * 80 + cursor_x;
    outb(0x3D4, 14);    // Tell the VGA board we are setting high cursor byte
    outb(0x3D5, cursorLocation >> 8);   // Send the high byte
    outb(0x3D4, 15);    // Tell the VGA board we are setting the low byte
    outb(0x3D5, cursorLocation);        // Send the low byte
}

/*
 * Scroll the text on the screen up by one line.
 */
static void scroll(void)
{
    u8int attributeByte = (0 /* black */ << 4) | (15 /* white */ & 0x0F);
    u16int blank = 0x20 /* space */ | (attributeByte << 8);

    if (cursor_y >= 25) {
        /* Move the current text chunk that makes up the screen back in the
         * buffer by a line. */
        int i;
        for (i = 0*80; i < 24*80; ++i) {
            video_memory[i] = video_memory[i + 80];
        }

        /* The last line should now be blank. Do this by writing 80 spaces
         * to it. */
        for (i = 24 * 80; i < 25 * 80; ++i) {
            video_memory[i] = blank;
        }

        /* Cursor should now be on the last line. */
        cursor_y = 24;
    }
}

void monitor_put(char c)
{
    u8int backColor = 0;    /* black */
    u8int foreColor = 15;   /* white */

    /* The attribute byte is made of two nibbles - the lower being the
     * foreground color, the upper the background color. */
    u8int attributeByte = (backColor << 4) | (foreColor & 0x0F);
    /* The attribute byte is the top 8 bits of the word we have to send to
     * the VGA board. */
    u16int attribute = attributeByte << 8;
    u16int *location;

    /* Handle a backspace by moving the cursor back one space. */
    if (c == 0x08 && cursor_x) {
        cursor_x--;
    }

    /* Handle a tab by increasing the cursor's X, but only to a point where
     * it is divisible by 8. */
    else if (c == 0x09) {
        cursor_x = (cursor_x + 8) & ~(8-1);
    }

    /* Handle a carriage return. */
    else if (c == '\r') {
        cursor_x = 0;
    }

    /* Handle a newline by moving cursor back to the left and increasing
     * the row */
    else if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    }

    /* Handle any other printable character. */
    else if (c >= ' ') {
        location = video_memory + (cursor_y * 80 + cursor_x);
        *location = c | attribute;
        cursor_x++;
    }

    /* Check if we need to insert a new line because we have reached the
     * end of the screen. */
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }

    /* Scroll the screen if needed. */
    scroll();
    /* Move the hardware cursor. */
    move_cursor();
}

void monitor_clear(void)
{
    u8int attributeByte = (0 /* black */ << 4) | (15 /* white */ & 0x0F);
    u16int blank = 0x20 /* space */ | (attributeByte << 8);

    int i;
    for (i = 0; i < 80 * 25; i++) {
        video_memory[i] = blank;
    }

    /* Move the hardware cursor back to the start. */
    cursor_x = 0;
    cursor_y = 0;
    move_cursor();
}

void monitor_write(const char *c)
{
    int i = 0;
    while (c[i]) {
        monitor_put(c[i++]);
    }
}

/*
 * Compute logarithm of a number in a specific base.
 * Result will be rounded up.
 */
static u32int log_base(u32int n, u32int base)
{
    u32int exp = 0;
    do {
        n /= base;
        exp++;
    } while (n);
    return exp;
}

void monitor_write_num(u32int n, u32int base, u8int c, u8int fwidth, char pad)
{
    u8int digit, printed = 0;
    /* Maximum multiple of base that fits into 32 bits. */
    u32int max = base == 16 ? 0x1000000 :
        (base == 10 ? 1000000000 : 010000000000);
    u8int len = log_base(n, base);
    while (fwidth-- > len) {
        monitor_put(pad);
    }

    while (max) {
        digit = (n / max) % base;
        if (printed || digit) {
            if (digit < 10)
                monitor_put(digit + '0');
            else
                monitor_put(digit - 10 + c);
            printed = 1;
        }
        max /= base;
    }
    if (!printed) {
        monitor_put('0');
    }
}

/*
 * Convert initial part of string to unsigned integer.
 * The leading digits in buffer will be skipped.
 */
static u8int atoi_skip(const char **buf)
{
    u8int val = 0;
    while (*buf[0] >= '0' && *buf[0] <= '9') {
        val = val * 10 + *buf[0] - '0';
        (*buf)++;
    }
    return val;
}

void monitor_print(const char *fmt, ...)
{
    va_list ap;
    u32int num;
    s32int snum;
    u8int fwidth, len;
    const char *str;
    char c, pad;

    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') {
            monitor_put(*fmt++);
            continue;
        }
        u8int use_upcase = 0;
        fmt++;

        pad = ' ';
        if (*fmt == '0')
            pad = '0';
        fwidth = atoi_skip(&fmt);

        switch (*fmt++) {
        case 'u':
            num = va_arg(ap, u32int);
            monitor_write_num(num, 10, '0', fwidth, pad);
            break;
        case 'X':
            use_upcase = 1;
        case 'x':
            num = va_arg(ap, u32int);
            monitor_write_num(num, 16, use_upcase ? 'A' : 'a', fwidth, pad);
            break;
        case 'o':
            num = va_arg(ap, u32int);
            monitor_write_num(num, 8, '0', fwidth, pad);
            break;
        case 'd':
        case 'i':
            snum = va_arg(ap, s32int);
            len = log_base(snum < 0 ? -snum : snum, 10) + (snum < 0 ? 1 : 0);
            while (fwidth-- > len)
                monitor_put(pad);
            if (snum < 0) {
                monitor_put('-');
                snum *= -1;
            }
            monitor_write_dec((u32int) snum);
            break;
        case '%':
            monitor_put('%');
            break;
        case 's':
            str = va_arg(ap, const char *);
            monitor_write(str);
            break;
        case 'c':
            c = va_arg(ap, int);
            monitor_put(c);
            break;
        default:
            PANIC("Unknown format flag");
        }
    }
    va_end(ap);
}
