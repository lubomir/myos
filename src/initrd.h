/**
 * @file    initrd.h
 *
 * Defines the interface for and structures relating to the initial ramdisk.
 *
 * Written for JamesM's kernel development tutorial.
 */

#ifndef INITRD_H
#define INITRD_H

#include "fs.h"

/**
 * Initialises the initial ramdisk.
 *
 * @param   address of the multiboot module
 * @return  completed filesystem node
 */
fs_node_t *initialise_initrd(u32int location);

#endif /* end of include guard: INITRD_H */
