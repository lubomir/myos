/*
 * kb.h -- defines functions for and structures relating to keyboard.
 * Taken from Bran's kernel development tutorial.
 */

#ifndef KB_H
#define KB_H

#include "common.h"

/* Current state of the keyboard - pressed modifier keys, state of lights.
 * These macros are used to access it. */
#define KB_STATE_CTRL       0x01
#define KB_STATE_ALT        0x02
#define KB_STATE_SHIFT      0x04
#define KB_STATE_SCRLOCK    0x08
#define KB_STATE_NUMLOCK    0x10
#define KB_STATE_CAPSLOCK   0x20

/* Variable defined in kb.c holding the state of keyboard. */
extern u8int kb_state;

/*
 * Register the keyboard interrupt handler.
 */
void initialise_keyboard(void);

#endif /* end of include guard: KB_H */
