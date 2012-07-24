/*
 * kb.h -- defines functions for and structures relating to keyboard.
 * Taken from Bran's kernel development tutorial.
 */

#ifndef KB_H
#define KB_H

#include "common.h"

/*
 * Register the keyboard interrupt handler.
 */
void initialise_keyboard(void);

#endif /* end of include guard: KB_H */
