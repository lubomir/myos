/*
 * paging.c -- defines functions for paging facilities
 */

#include <string.h>

#include "kheap.h"
#include "monitor.h"
#include "paging.h"

/* The kernel's page directory. */
page_directory_t *kernel_directory = 0;

/* The current page directory. */
page_directory_t *current_directory = 0;

/* A bitset of frames - used or free. */
u32int *frames;
u32int nframes;

/* defined in kheap.c */
extern u32int placement_address;

/* Macros used in the bitset algorithms. */
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

/*
 * Static function to set a bit in the frames bitset.
 */
static void set_frame(u32int frame_addr)
{
    u32int fram = frame_addr/0x1000;
    u32int idx = INDEX_FROM_BIT(fram);
    u32int off = OFFSET_FROM_BIT(fram);
    frames[idx] |= (0x1 << off);
}

/*
 * Static function to clear a bit in the frames bitset.
 */
static void clear_frame(u32int frame_addr)
{
    u32int fram = frame_addr/0x1000;
    u32int idx = INDEX_FROM_BIT(fram);
    u32int off = OFFSET_FROM_BIT(fram);
    frames[idx] &= ~(0x1 << off);
}

/*
 * Static function to find the first free frame.
 */
static u32int first_frame(void)
{
    /* This function runs in constant time, because it remembers its
     * previous state in static variables.
     *
     * This is even faster than skipping by comparing frames with 0xFFFFFFFF.
     */
    static u32int i = 0, j = 0;
    for (; i < INDEX_FROM_BIT(nframes); i++) {
        for (; j < 32; j++) {
            if (!(frames[i] & (0x1 << j))) {
                return i * 4 * 8 + j;
            }
        }
        j = 0;
    }
    PANIC("No free frame!");
}

/*
 * Function to allocate a frame.
 */
void alloc_frame(page_t *page, int is_kernel, int is_writable)
{
    if (page->frame != 0) {
        return;     /* Frame was already allocated, return straight away. */
    }
    u32int idx = first_frame(); /* idx is now the index of the first frame */
    if (idx == (u32int) - 1) {
        PANIC("No free frames!");
    }
    set_frame(idx * 0x1000);    /* this frame is ours now */
    page->present   = 1;
    page->rw        = is_writable ? 1 : 0;
    page->user      = is_kernel ? 0 : 1;
    page->frame     = idx;
}

/*
 * Function to deallocate a frame.
 */
void free_frame(page_t *page)
{
    u32int frame = page->frame;
    if (!frame) {
        return;     /* The given page didn't actually have an allocated frame */
    }
    clear_frame(frame);
    page->frame = 0x0;
}

void initialise_paging(void)
{
    /* The size of physical memory. For the moment we assume it
     * is 16 MB big. */
    u32int mem_end_page = 0x1000000;

    nframes = mem_end_page / 0x1000;
    frames = (u32int *) kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));

    /* Let's make a page directory. */
    kernel_directory = (page_directory_t *) kmalloc_a(sizeof(page_directory_t));
    current_directory = kernel_directory;

    /* We need to identity map (phys addr = virt addr) from 0x0 to the end
     * of the used memory, so we can access this transparently, as if paging
     * wasn't enabled.
     * NOTE that we use a while loop here deliberately, inside the loop body
     * we actually change placement_address by calling kmalloc(). A while
     * loop causes this to be computed on-the-fly rather than once at the
     * start. */
    int i = 0;
    while (i < placement_address) {
        /* Kernel code is readable but not writable from userspace. */
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    /* Before we enable paging, we must register our page fault handler. */
    register_interrupt_handler(14, page_fault);

    /* Now, enable paging! */
    switch_page_directory(kernel_directory);
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    asm volatile ("mov %0, %%cr3" :: "r"(&dir->tablesPhysical));
    u32int cr0;
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  /* Enable paging! */
    asm volatile ("mov %0, %%cr0" :: "r"(cr0));
}

page_t *get_page(u32int address, int make, page_directory_t *dir)
{
    /* Turn the address into an index. */
    address /= 0x1000;
    /* Find the page table containing this address. */
    u32int table_idx = address / 1024;
    if (dir->tables[table_idx]) {
        return &dir->tables[table_idx]->pages[address % 1024];
    }
    if (make) {
        u32int tmp;
        dir->tables[table_idx] =
            (page_table_t *) kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_idx], 0, 0x1000);
        dir->tablesPhysical[table_idx] = tmp | 0x7; /* PRESENT, RW, US */
        return &dir->tables[table_idx]->pages[address % 1024];
    }
    return 0;
}

void page_fault(registers_t regs)
{
    /* A page fault has occurred.
     * The faulting address is stored in the CR2 register. */
    u32int faulting_address;
    asm volatile ("mov %%cr2, %0" : "=r"(faulting_address));

    /* The error code gives us details of what happened. */
    int present  = !(regs.err_code & 0x1);  /* Page not present */
    int rw       = regs.err_code & 0x2;     /* Write operation? */
    int us       = regs.err_code & 0x4;     /* Processor was in user-mode? */
    int reserved = regs.err_code & 0x8;     /* Overwritten CPU-reseved bits
                                               of page entry? */
#if 0
    int id       = regs.err_code & 0x10;    /* Caused by an instruction
                                               fetch? */
#endif

    /* Output an error message. */
    monitor_write("Page fault! ( ");
    if (present) {
        monitor_write("present ");
    }
    if (rw) {
        monitor_write("read-only ");
    }
    if (us) {
        monitor_write("user-mode ");
    }
    if (reserved) {
        monitor_write("reserved ");
    }
    monitor_write(") at 0x");
    monitor_write_hex(faulting_address);
    monitor_write("\n");
    PANIC("Page fault!");
}
