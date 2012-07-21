/*
 * monitor.h -- defines the interface for monitor.h
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
 * Write a number to monitor in hexadecimal system.
 */
void monitor_write_hex(u32int n);

/*
 * Write a number to monitor in decimal system.
 */
void monitor_write_dec(u32int n);

/*
 * Printf-like function for outputting text to monitor.
 * This function supports following formatting flags:
 *   u      print unsigned in decimal
 *   x      print unsigned in hexadecimal
 *   d, i   print signed in signed decimal notation
 *   s      print string
 *   c      print char
 *   %      print plain %
 */
void monitor_print(const char *fmt, ...)
    __attribute__((format(printf, 1, 2)));

#endif /* end of include guard: MONITOR_H */
