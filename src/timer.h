/*
 * timer.h -- defines the interface for all PIT related functions.
 * Written and taken from JamesM's kernel development tutorial.
 */

#ifndef TIMER_H
#define TIMER_H

#include "common.h"

/*
 * The Programmable Interval Timer is a chip connected to IRQ0.
 *
 * It can interrupt the CPU at specified intervals.
 */

/*
 * This is the frequency of PIT internal clock.
 */
#define PIT_FREQ    1193180

/*
 * These are the I/O ports for setting the PIT.
 *
 * Each 8 bit data port is the same, and is used to set the counter's 16 bit
 * reload value or read the channel's 16 bits current count.
 *
 * The Mode/Command register at I/O address PIT_CTRL contains the following:
 * Bits         Usage
 *  6 and 7     Select channel:
 *                  0 0 = Channel 0
 *                  0 1 = Channel 1
 *                  1 0 = Channel 2
 *                  1 1 = Read-back command (8254 only)
 *  4 and 5     Access mode:
 *                  0 0 = Latch count value command
 *                  0 1 = Access mode: lobyte only
 *                  1 0 = Access mode: hibyte only
 *                  1 1 = Access mode: lobyte/hibyte
 *  1 to 3      Operating mode:
 *                  0 0 0 = Mode 0 (interrupt on terminal count)
 *                  0 0 1 = Mode 1 (hardware re-triggerable one-shot)
 *                  0 1 0 = Mode 2 (rate generator)
 *                  0 1 1 = Mode 3 (square wave generator)
 *                  1 0 0 = Mode 4 (software triggered strobe)
 *                  1 0 1 = Mode 5 (hardware triggered strobe)
 *                  1 1 0 = Mode 2 (same as 010b)
 *                  1 1 1 = Mode 3 (same as 011b)
 *  0           BCD/Binary mode:
 *                  0 = 16-bit binary
 *                  1 = four-digit BCD
 *
 *  Use following macros to set the desired bytes.
 */
#define PIT_DATA_0  0x40
#define PIT_DATA_1  0x41
#define PIT_DATA_2  0x42
#define PIT_CTRL    0x43

#define PIT_CHANNEL_0   0x00
#define PIT_CHANNEL_1   0x80
#define PIT_CHANNEL_2   0x40
#define PIT_AM_LATCH    0x00
#define PIT_AM_LO       0x20
#define PIT_AM_HI       0x10
#define PIT_AM_LOHI     0x30
#define PIT_MODE_0      0x00
#define PIT_MODE_1      0x08
#define PIT_MODE_2      0x04
#define PIT_MODE_3      0x0C
#define PIT_MODE_4      0x02
#define PIT_MODE_5      0x0A
#define PIT_BINARY      0x00
#define PIT_BCD         0x01

/*
 * Initialize timer to fire with given frequency.
 * NOTE: interrupts must be enabled for timer to work. Use 'sti' instruction
 * to enable them.
 *
 * @param frequency     requested frequency in Hz
 */
void init_timer(u32int frequency);

#endif /* end of include guard: TIMER_H */
