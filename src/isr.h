/**
 * @file    isr.h
 *
 * Interface and structures for high level interrupt service routines
 *
 * Part of this code is modified from Bran's kernel development tutorials.
 * Rewritten for and taken from JamesM's kernel development tutorials.
 */

#ifndef ISR_H
#define ISR_H

#include "common.h"

/**
 * This structure represents contents of registers.
 * It is passed to interrupt handlers.
 */
typedef struct {
    /** Data segment selector. */
    u32int ds;
    /* Pushed by pusha */
    u32int edi /** Contents of edi */, esi /** Contents of esi */,
           ebp /** Contents of ebp */, esp /** Contents of esp */,
           ebx /** Contents of ebx */, edx /** Contents of edx */,
           ecx /** Contents of ecx */, eax /** Contents of eax */;
    /** Interrupt number. */
    u32int int_no;
    /** Error code (if applicable). */
    u32int err_code;
    /* Pushed by the processor automatically. */
    u32int eip /** Instruction pointer */, cs /** Code segment selector */,
           eflags /** Flags of the CPU */, useresp /** User stack pointer */,
           ss /** Stack segment */;
} registers_t;

#define IRQ0    32      /**< Interrupt request 0  */
#define IRQ1    33      /**< Interrupt request 1  */
#define IRQ2    34      /**< Interrupt request 2  */
#define IRQ3    35      /**< Interrupt request 3  */
#define IRQ4    36      /**< Interrupt request 4  */
#define IRQ5    37      /**< Interrupt request 5  */
#define IRQ6    38      /**< Interrupt request 6  */
#define IRQ7    39      /**< Interrupt request 7  */
#define IRQ8    40      /**< Interrupt request 8  */
#define IRQ9    41      /**< Interrupt request 9  */
#define IRQ10   42      /**< Interrupt request 10 */
#define IRQ11   43      /**< Interrupt request 11 */
#define IRQ12   44      /**< Interrupt request 12 */
#define IRQ13   45      /**< Interrupt request 13 */
#define IRQ14   46      /**< Interrupt request 14 */
#define IRQ15   47      /**< Interrupt request 15 */

/**
 * Enables registration of callbacks for interrupts and IRQs.
 * For IRQs, to ease the confusion, use #defines above as the first parameter.
 */
typedef void (*isr_t)(registers_t*);

/**
 * Set a handler for given interrupt.
 * @param n         interrupt number
 * @param handler   function to be executed for that interrupt
 */
void register_interrupt_handler(u8int n, isr_t handler);

#endif /* end of include guard: ISR_H */
