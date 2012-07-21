/*
 * isr.h -- Interface and structures for high level interrupt service routines
 * Part of this code is modified from Bran's kernel development tutorials.
 * Rewritten and taken from JamesM's kernel development tutorials.
 */

#ifndef ISR_H
#define ISR_H

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

#define IRQ0    32
#define IRQ1    33
#define IRQ2    34
#define IRQ3    35
#define IRQ4    36
#define IRQ5    37
#define IRQ6    38
#define IRQ7    39
#define IRQ8    40
#define IRQ9    41
#define IRQ10   42
#define IRQ11   43
#define IRQ12   44
#define IRQ13   45
#define IRQ14   46
#define IRQ15   47

/*
 * Enables registration of callbacks for interrupts and IRQs.
 * For IRQs, to ease the confusion, use #defines above as the first parameter.
 */
typedef void (*isr_t)(registers_t);
void register_interrupt_handler(u8int n, isr_t handler);

#endif /* end of include guard: ISR_H */
