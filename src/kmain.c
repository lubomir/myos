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
#include "kheap.h"
#include "monitor.h"
#include "multiboot.h"
#include "paging.h"
#include "task.h"
#include "timer.h"

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

    /* Create a new process in a new address space which is a clone of this. */
    int ret = fork();

    monitor_print("fork() returned %x, and getpid() returned %x\n",
            ret, getpid());
    monitor_write("===============================================================================\n");

    /* The next section is not reentrant. */
    asm volatile ("cli");

    int i = 0;
    struct dirent *node = 0;
    while ((node = readdir_fs(fs_root, i)) != 0) {
        monitor_print("Found file %s\n", node->name);
        fs_node_t *fsnode = finddir_fs(fs_root, node->name);

        if ((fsnode->flags & 0x7) == FS_DIRECTORY) {
            monitor_write("\t(directory)\n");
        } else {
            monitor_print("\tlength %u bytes\n", fsnode->length);
        }
        i++;
    }

    asm volatile ("sti");

    return 0;
}
