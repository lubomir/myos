/*
 * initrd.h -- Defines the interface for and structures relating to the
 *             initial ramdisk.
 * Written for JamesM's kernel development tutorial.
 */

#ifndef INITRD_H
#define INITRD_H

#include "common.h"
#include "fs.h"

typedef struct {
    u32int nfiles;
} initrd_header_t;

typedef struct {
    /* Magic number for error checking. */
    u8int magic;
    /* Filename. */
    u8int name[64];
    /* Offset in the initrd where the file starts. */
    u32int offset;
    /* Length of the file. */
    u32int length;
} initrd_file_header_t;

/*
 * Initialises the initial ramdisk. It gets passed the address of the
 * multiboot module, and returns a completed filesystem node.
 */
fs_node_t *initialise_initrd(u32int location);

#endif /* end of include guard: INITRD_H */
