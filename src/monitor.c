/*
 * monitor.c -- defines functions to control the monitor.
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

void monitor_write_hex(u32int n)
{
    u32int quot = 0x10000000;
    u8int digit, printed = 0;
    while (quot) {
        digit = (n / quot) % 0x10;
        if (printed || digit) {
            if (digit < 10) {
                monitor_put(digit + '0');
            } else {
                monitor_put(digit - 10 + 'a');
            }
            printed = 1;
        }
        quot = quot / 0x10;
    }
    if (!printed) {
        monitor_put('0');
    }
}

void monitor_write_dec(u32int n)
{
    u32int quot = 1000000000;
    u8int digit, printed = 0;
    while (quot) {
        digit = (n / quot) % 10;
        if (printed || digit) {
            monitor_put(digit + '0');
            printed = 1;
        }
        quot = quot / 10;
    }
    if (!printed) {
        monitor_put('0');
    }
}

void monitor_print(const char *fmt, ...)
{
    va_list ap;
    u32int num;
    const char *str;
    char c;

    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') {
            monitor_put(*fmt++);
            continue;
        }
        fmt++;
        switch (*fmt++) {
        case 'u':
            num = va_arg(ap, u32int);
            monitor_write_dec(num);
            break;
        case 'x':
            num = va_arg(ap, u32int);
            monitor_write_hex(num);
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
