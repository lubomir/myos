/*
 * main.c -- defines the C-code kernel entry point, calls the initialization
 * routines.
 * Copied from JamesM's tutorials.
 */

#include <string.h>

#include "common.h"
#include "descriptor-tables.h"
#include "monitor.h"
#include "paging.h"
#include "timer.h"

extern u32int placement_address;

int kmain(struct multiboot *mboot_ptr)
{
    init_descriptor_tables();
    monitor_clear();
    placement_address *= 2;
    initialise_paging();

    monitor_write("Hello, paging world!\n");

    u32int *ptr = (u32int *) 0xA0000000;
    u32int do_page_fault = *ptr;

    return 0;
}
