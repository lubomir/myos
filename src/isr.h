/*
 * isr.h -- Interface and structures for high level interrupt service routines
 * Part of this code is modified from Bran's kernel development tutorials.
 * Rewritten and taken from JamesM's kernel development tutorials.
 */

#include "common.h"

typedef struct registers {
    /* Data segment selector */
    u32int ds;
    /* Pushed by pusha */
    u32int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    /* Interrupt number and error code (if applicable) */
    u32int int_no, err_code;
    /* Pushed by the processor automatically */
    u32int eip, cs, eflags, useresp, ss;
} registers_t;
