/*
 * paging.h -- defines structures and interface to paging facilities
 */

#ifndef PAGING_H
#define PAGING_H

#include "common.h"
#include "isr.h"


struct page {
    /* Page present in memory */
    u32int present  : 1;
    /* Read-only if clear, read-write if set */
    u32int rw       : 1;
    /* Supervisor level only if clear */
    u32int user     : 1;
    /* Has the page been accessed since last refresh? */
    u32int access   : 1;
    /* Has the page been written to since last refresh? */
    u32int dirty    : 1;
    /* Amalgamation of unused and reserved bits */
    u32int unused   : 7;
    /* Frame address (shifted right 12 bits) */
    u32int frame     : 20;
};

typedef struct page page_t;

typedef struct page_table {
    page_t pages[1024];
} page_table_t;

struct page_directory {
    /* Array of pointers to pagetables */
    page_table_t *tables[1024];
    /* Array of pointers to page tables above, but gives their physical
     * location, for loading into CR3 register. */
    u32int tablesPhysical[1024];
    /* The physical address of tablesPhysical. This comes into play when
     * we get our kernel heap allocated and the directory may be in a
     * different location in virtual memory. */
    u32int physicalAddr;
};

typedef struct page_directory page_directory_t;

/*
 * Sets up the environment, page directories, etc. and enables paging.
 */
void initialise_paging(void);

/*
 * Causes the specified page directory to be loaded into the CR3 register.
 */
void switch_page_directory(page_directory_t *newdir);

/*
 * Retrieve a pointer to the page required. If make is set, create the page
 * if it does not exist already.
 */
page_t *get_page(u32int address, int make, page_directory_t *dir);

/*
 * Handler for page faults.
 */
void page_fault(registers_t regs);

/*
 * Function to allocate a frame.
 */
void alloc_frame(page_t *page, int is_kernel, int is_writable);

/*
 * Function to deallocate a frame.
 */
void free_frame(page_t *page);

#endif /* end of include guard: PAGING_H */
