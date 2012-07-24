/*
 * main.c -- defines the C-code kernel entry point, calls the initialization
 * routines.
 * Copied from JamesM's tutorials.
 */

#include <string.h>

#include "common.h"
#include "descriptor-tables.h"
#include "fs.h"
#include "initrd.h"
#include "kb.h"
#include "kheap.h"
#include "monitor.h"
#include "multiboot.h"
#include "paging.h"
#include "task.h"
#include "timer.h"
#include "syscall.h"

extern u32int placement_address;

/*
 * Address where GRUB put the initial stack.
 */
u32int initial_esp;

int kmain(struct multiboot *mboot_ptr, u32int initial_stack)
{
    initial_esp = initial_stack;
    init_descriptor_tables();
    monitor_clear();
    /* Initialise the PIT to 100 Hz. */
    asm volatile ("sti");
    init_timer(50);

    /* Find the location of our initial ramdisk. */
    ASSERT(mboot_ptr->mods_count > 0);
    u32int initrd_location = *((u32int*)mboot_ptr->mods_addr);
    u32int initrd_end = *((u32int*)(mboot_ptr->mods_addr+4));

    /* Do not trample our module with placement accesses, please! */
    placement_address = initrd_end;

    /* Start paging. */
    initialise_paging();

    /* Start multitasking. */
    initialise_tasking();

    /* Initialise the initial ramdisk, and set it as the filesystem root. */
    fs_root = initialise_initrd(initrd_location);

    initialise_keyboard();

    return 0;
}
