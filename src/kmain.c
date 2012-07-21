/*
 * main.c -- defines the C-code kernel entry point, calls the initialization
 * routines.
 * Copied from JamesM's tutorials.
 */

#include <string.h>

#include "common.h"
#include "descriptor-tables.h"
#include "kheap.h"
#include "monitor.h"
#include "paging.h"
#include "timer.h"

extern u32int placement_address;

int kmain(struct multiboot *mboot_ptr)
{
    init_descriptor_tables();
    monitor_clear();
    placement_address *= 2;

    monitor_write("Hello, paging world with heap!\n");

    u32int a = kmalloc(8);
    initialise_paging();
    u32int b = kmalloc(8);
    u32int c = kmalloc(8);

    monitor_print("a: %x, b: %x\nc: %x", a, b, c);
    kfree(b);
    kfree(c);
    u32int d = kmalloc(12);
    monitor_print(", d: %x\n", d);

    return 0;
}
