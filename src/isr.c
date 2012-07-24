/*
 * isr.c -- High level interrupt service routines and interrupt request
 * handlers.
 * Part of this code is modified from Bran's kernel development tutorials.
 * Rewritten and taken from JamesM's kernel development tutorials.
 */

#include "common.h"
#include "isr.h"
#include "monitor.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(u8int n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

/*
 * This gets called from our ASM interrupt handler stub.
 */
void isr_handler(registers_t regs)
{
    /* This line is important. When the processor extends the 8-bit interrupt
     * number to a 32 bit value, it sign-extends, not zero extends. So fi the
     * most significant bit (0x80) is set, regs.int_no will be very large
     * (about 0xFFFFFF80). */
    u8int int_no = regs.int_no & 0xFF;
    if (interrupt_handlers[regs.int_no]) {
        isr_t handler = interrupt_handlers[int_no];
        handler(regs);
    } else {
        monitor_print("Unhandled interrupt: %u\n", int_no);
    }
}

/*
 * This gets called from our ASM interrupt handler stub.
 */
void irq_handler(registers_t regs)
{
    /* Send an EOI (end-of-interrupt) signal to PICs. */
    if (regs.int_no >= 40) {    /* If this interrupt involved the slave */
        outb(PIC2, EOI);
    }
    outb(PIC1, EOI);

    if (interrupt_handlers[regs.int_no]) {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
}
