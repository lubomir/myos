/*
 * kb.c -- implementation of basic keyboard driver.
 * Taken from Bran's kernel development tutorial.
 */

#include "isr.h"
#include "kb.h"
#include "monitor.h"

/* US Keyboard Layout. */
u8int kdbus[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b' /* backspace */,
    '\t' /* tab */, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']', '\n' /* enter key */,
    0 /* 29 - Control */,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0 /* Left shift */,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0 /* right shift */,
    '*',
    0 /* alt */,
    ' ' /* space bar */,
    0 /* caps lock */,
    0 /* 59 - f1 key ...> */,
    0, 0, 0, 0, 0, 0, 0, 0,
    0 /* <... F10 */,
    0 /* 69 - Num Lock */,
    0 /* Scroll Lock */,
    0 /* Home */,
    0 /* Up Arrow */,
    0 /* Page Up */,
    '-',
    0 /* Left Arrow */,
    0,
    0 /* Right Arrow */,
    '+',
    0 /* 79 - End key */,
    0 /* Down arrow */,
    0 /* Page Down */,
    0 /* Insert */,
    0 /* Delete */,
    0, 0, 0,
    0 /* F11 */,
    0 /* F12 */,
    0, /* All other keys are undefined */
};

/* Bitfield representing the state of a keyboard. */
u8int kb_state;

#define KB_DATA 0x60
#define KB_CTRL 0x64

void keyboard_handler(registers_t regs)
{
    u8int scancode, key;

    /* Read from the keyboard's data buffer. */
    scancode = inb(KB_DATA);

    /* If the top bit of the byte we read from the keyboard is set,
     * that means that a key has just been released. */
    if (scancode & 0x80) {
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
            key = kdbus[scancode];
            if (key >= 'a' && key <= 'z' && (kb_state & KB_STATE_SHIFT))
                key -= 32;
            monitor_put(key);
        }
    }
}

void initialise_keyboard(void)
{
    register_interrupt_handler(IRQ1, keyboard_handler);
}
