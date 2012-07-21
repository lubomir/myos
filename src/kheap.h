/*
 * kheap.h -- defines interface to basic kernel heap
 */

#ifndef KHEAP_H
#define KHEAP_H

#include "common.h"
#include "ordered-array.h"

#define KHEAP_START         0xC0000000
#define KHEAP_INIT_SIZE     0x100000
#define HEAP_INDEX_SIZE    0x20000
#define HEAP_MAGIC          0x123890AB
#define HEAP_MIN_SIZE       0x70000

typedef struct {
    /* Magic number, used for error checking and identification. */
    u32int magic;
    /* 1 if this is a hole, 0 if this is a block. */
    u8int is_hole;
    /* Size of this block, including the end footer. */
    u32int size;
} header_t;

typedef struct {
    /* Magic number, same as in header_t. */
    u32int magic;
    /* Pointer to the block header. */
    header_t *header;
} footer_t;

typedef struct {
    ordered_array_t index;
    /* The start of our allocated space. */
    u32int start_addr;
    /* The end of our allocated space. May be expanded up to max_address. */
    u32int end_addr;
    /* The maximum address heap can be expanded to. */
    u32int max_addr;
    /* Should extra pages requested be mapped as supervisor-ony? */
    u8int supervisor;
    /* Should extra pages requested be mapped as read-only? */
    u8int readonly;
} heap_t;

/*
 * Create a new heap.
 */
heap_t * heap_create(u32int start, u32int end, u32int max,
                     u8int supervisor, u8int readonly);

/*
 * Allocates a contiguous region of memory 'size' in size. If page_align
 * is set, the block will start on page boundary.
 */
void * alloc(heap_t *heap, u32int size, u8int page_align);

/*
 * Releases a block allocated with 'alloc'.
 */
void free(heap_t *heap, void *p);

/*
 * Allocate block of sz bytes. If align is nonzero, page align it. If phys
 * is present, store physical address there.
 */
u32int kmalloc_internal(u32int sz, int align, u32int *phys);

/*
 * Allocate sz bytes of memory and return its address.
 */
u32int kmalloc(u32int sz);

/*
 * Free memory allocated with kmalloc after heap was enabled.
 */
void kfree(u32int p);

/*
 * Allocate page aligned memory of size sz.
 */
u32int kmalloc_a(u32int sz);

/*
 * Allocate sz bytes and return physical address.
 */
u32int kmalloc_p(u32int sz, u32int *phys);

/*
 * Allocate page aligned block and return physical address.
 */
u32int kmalloc_ap(u32int sz, u32int *phys);

#endif /* end of include guard: KHEAP_H */
