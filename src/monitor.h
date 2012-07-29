/*
 * monitor.h -- Defines the interface for printing to monitor.
 * From JamesM's kernel development tutorials.
 */

#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"

/*
 * Write a single character out to the screen.
 */
void monitor_put(char c);

/*
 * Clear the screen to all black.
 */
void monitor_clear(void);

/*
 * Output a zero-terminated ASCII string to the monitor.
 */
void monitor_write(const char *c);

/*
 * Write a number to monitor in given base. 'c' is the character that will
 * be printed for digit 10. The 'fwidth' argument specifies length of the
 * printed string. If number would be shorted than 'fwidth', it will be left
 * padded with character pad.
 */
void monitor_write_num(u32int n, u32int base, u8int c, u8int fwidth, char pad);

/*
 * Write a number to monitor in hexadecimal system.
 */
#define monitor_write_hex(n) monitor_write_num(n, 16, 'a', 0, ' ')

/*
 * Write a number to monitor in decimal system.
 */
#define monitor_write_dec(n) monitor_write_num(n, 10, '0', 0, ' ')

/*
 * Printf-like function for outputting text to monitor.
 * This function supports following formatting flags:
 *   u      print unsigned in decimal
 *   x, X   print unsigned in hexadecimal
 *   o      print unsigned in octal
 *   d, i   print signed in signed decimal notation
 *   s      print string
 *   c      print char
 *   %      print plain %
 * All number formatting flags can optionally contain field width specifier
 * as a standard printf().
 */
void monitor_print(const char *fmt, ...)
    __attribute__((format(printf, 1, 2)));

#endif /* end of include guard: MONITOR_H */
