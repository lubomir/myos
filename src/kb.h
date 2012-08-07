/**
 * @file    kb.h
 *
 * Defines functions for and structures relating to keyboard.
 *
 * Adapted from Bran's kernel development tutorial.
 */

#ifndef KB_H
#define KB_H

#include "common.h"

/* Current state of the keyboard - pressed modifier keys, state of lights. */
#define KB_STATE_CTRL       0x01    /**< Control key pressed */
#define KB_STATE_ALT        0x02    /**< Alt key pressed */
#define KB_STATE_SHIFT      0x04    /**< Shift key pressed */
#define KB_STATE_SCRLOCK    0x08    /**< Scroll lock key pressed */
#define KB_STATE_NUMLOCK    0x10    /**< Num lock key pressed */
#define KB_STATE_CAPSLOCK   0x20    /**< Caps lock key pressed */

/** Variable defined in src/kb.c holding the state of keyboard.
 * Use KB_STATE_* macros to access it. */
extern u8int kb_state;

/**
 * Register the keyboard interrupt handler with given keyboard map.
 * For description of keyboard map format, see docs/keymap.txt.
 *
 * @param map   keybord map to use
 */
void initialise_keyboard(u8int *map);

#endif /* end of include guard: KB_H */
