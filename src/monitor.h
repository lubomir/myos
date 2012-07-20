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

#endif /* end of include guard: MONITOR_H */
