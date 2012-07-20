/*
 * main.c -- defines the C-code kernel entry point, calls the initialization
 * routines.
 * Copied from JamesM's tutorials.
 */

#include "common.h"
#include "descriptor-tables.h"
#include "monitor.h"
#include "timer.h"


int kmain(struct multiboot *mboot_ptr)
{
    init_descriptor_tables ();
    monitor_clear();

    asm volatile ("int $0x3");
    asm volatile ("int $0x4");

    /* Without enabling interrupts timer will never kick in. */
    asm volatile ("sti");

    init_timer(50);

    return 0xDEADBABA;
}
