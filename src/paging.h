/**
 * @file    paging.h
 *
 * Defines structures and interface to paging facilities
 *
 * Taken from JamesM's kernel development tutorial.
 */

#ifndef PAGING_H
#define PAGING_H

#include "common.h"
#include "isr.h"

/** Representation of a page. */
typedef struct {
    /** Page present in memory */
    u32int present  : 1;
    /** Read-only if clear, read-write if set */
    u32int rw       : 1;
    /** Supervisor level only if clear */
    u32int user     : 1;
    /** Has the page been accessed since last refresh? */
    u32int access   : 1;
    /** Has the page been written to since last refresh? */
    u32int dirty    : 1;
    /** Amalgamation of unused and reserved bits */
    u32int unused   : 7;
    /** Frame address (shifted right 12 bits) */
    u32int frame    : 20;
} page_t;

/** Page table is an array of 1024 pages. */
typedef struct {
    page_t pages[1024];     /**< Pages in this table */
} page_table_t;

/**
 * Page directory contains array of page tables. */
typedef struct {
    /** Array of pointers to pagetables */
    page_table_t *tables[1024];
    /** Array of pointers to page tables above, but gives their physical
     * location, for loading into CR3 register. */
    u32int tablesPhysical[1024];
    /** The physical address of tablesPhysical. This comes into play when
     * we get our kernel heap allocated and the directory may be in a
     * different location in virtual memory. */
    u32int physicalAddr;
} page_directory_t;

/**
 * Sets up the environment, page directories, etc. and enables paging.
 */
void initialise_paging(void);

/**
 * Causes the specified page directory to be loaded into the CR3 register.
 *
 * @param newdir    page directory to use from now on
 */
void switch_page_directory(page_directory_t *newdir);

/**
 * Retrieve a pointer to the page required.
 *
 * @param address   address of which to get the page
 * @param make      if nonzero and page does not exist, create it
 * @param dir       page directory to retrieve from
 * @return page where the address is located
 */
page_t *get_page(u32int address, int make, page_directory_t *dir);

/**
 * Handler for page faults.
 *
 * @param regs  state of registers
 */
void page_fault(registers_t *regs);

/**
 * Allocate a frame.
 */
void alloc_frame(page_t *page, int is_kernel, int is_writable);

/**
 * Function to deallocate a frame.
 */
void free_frame(page_t *page);

/**
 * Clone a page directory.
 *
 * @param src   directory to be cloned
 * @return clone of src
 */
page_directory_t *clone_directory(page_directory_t *src);

#endif /* end of include guard: PAGING_H */
