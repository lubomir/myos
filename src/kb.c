/*
 * kb.c -- Provides implementation of basic keyboard driver.
 * Adapted from Bran's kernel development tutorial.
 */

#include <string.h>

#include "isr.h"
#include "kb.h"
#include "kheap.h"
#include "monitor.h"

static u8int *kbmap;

/* Bitfield representing the state of a keyboard. */
u8int kb_state;

#define KB_DATA 0x60
#define KB_CTRL 0x64

static void keyboard_handler(registers_t *regs)
{
    /* Read from the keyboard's data buffer. */
    u8int scancode = inb(KB_DATA);

    /* If the top bit of the byte we read from the keyboard is set,
     * that means that a key has just been released. */
    if (scancode & 0x80) {
        scancode &= ~0x80;
        /* Do something about it. */
        switch (scancode) {
        case 0x2A: /* Left Shift */
        case 0x36: /* Right Shift */
            kb_state &= ~KB_STATE_SHIFT;
            break;
        case 0x38: /* Left Alt */
            kb_state &= ~KB_STATE_ALT;
            break;
        case 0x1D: /* Left Control */
            kb_state &= ~KB_STATE_CTRL;
            break;
        }
    } else {
        /* Here a key was just pressed. Please note that if you hold a key
         * down, you will get repeated key press interrupts. */
        switch (scancode) {
        case 0x2A: /* Left Shift */
        case 0x36: /* Right Shift */
            kb_state |= KB_STATE_SHIFT;
            break;
        case 0x38: /* Left Alt */
            kb_state |= KB_STATE_ALT;
            break;
        case 0x1D: /* Left Control */
            kb_state |= KB_STATE_CTRL;
            break;
        default:
            monitor_put(kbmap[scancode + (kb_state & KB_STATE_SHIFT ? 128 : 0)]);
        }
    }
}

void initialise_keyboard(u8int *map)
{
    kbmap = kmalloc(256);
    memcpy(kbmap, map, 256);
    register_interrupt_handler(IRQ1, &keyboard_handler);
}
