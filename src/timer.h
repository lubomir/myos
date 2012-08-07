/**
 * @file    timer.h
 *
 * Defines the interface for all PIT related functions.
 *
 * The Programmable Interval Timer is a chip connected to IRQ0.
 * It can interrupt the CPU at specified intervals.
 *
 * Written and taken from JamesM's kernel development tutorial.
 */

#ifndef TIMER_H
#define TIMER_H

#include "common.h"

/**
 * Initialize timer to fire with given frequency.
 * NOTE: interrupts must be enabled for timer to work. Use `sti` instruction
 * to enable them.
 *
 * @param frequency     requested frequency in Hz
 */
void init_timer(u32int frequency);

#endif /* end of include guard: TIMER_H */
