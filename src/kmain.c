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
#include "timer.h"

extern u32int placement_address;

int kmain(struct multiboot *mboot_ptr)
{
    init_descriptor_tables();
    monitor_clear();

    ASSERT(mboot_ptr->mods_count > 0);
    u32int initrd_location = *((u32int*)mboot_ptr->mods_addr);
    u32int initrd_end = *((u32int*)(mboot_ptr->mods_addr+4));

    /* Do not trample our module with placement accesses, please! */
    placement_address = initrd_end;

    /* Start paging. */
    initialise_paging();

    /* Initialise the initial ramdisk, and set it as the filesystem root. */
    fs_root = initialise_initrd(initrd_location);

    int i = 0;
    struct dirent *node = 0;
    while ((node = readdir_fs(fs_root, i)) != 0) {
        monitor_print("Found file %s\n", node->name);
        fs_node_t *fsnode = finddir_fs(fs_root, node->name);

        if ((fsnode->flags & 0x7) == FS_DIRECTORY) {
            monitor_write("\t(directory)\n");
        } else {
            monitor_print("\tlength %d bytes\n", fsnode->length);
        }
        i++;
    }
    return 0;
}
