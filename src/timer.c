/*
 * timer.c -- initialises the PIT, and handles clock updates.
 * Written and taken from JamesM's kernel development tutorial.
 */

#include "timer.h"
#include "isr.h"
#include "task.h"

u32int tick = 0;

static void timer_callback(registers_t regs)
{
    tick++;
    switch_task();
}

void init_timer(u32int frequency)
{
    /* Register the callback. */
    register_interrupt_handler(IRQ0, &timer_callback);

    /* This is the value to be sent to PIT to get required frequency.
     * Important to note is that the divisor must be small enough to fit
     * into 16 bits. */
    u32int divisor = PIT_FREQ / frequency;

    /* Send the command byte. */
    outb(PIT_CTRL, PIT_BINARY | PIT_MODE_2 | PIT_AM_LOHI | PIT_CHANNEL_0);

    /* Divisor will be sent byte-wise. */
    outb(PIT_DATA_0, divisor & 0xFF);           /* Low byte */
    outb(PIT_DATA_0, (divisor >> 8) & 0xFF);    /* High byte */
}
