/**
 * @file kheap.h
 *
 * Defines interface to basic kernel heap.
 *
 * Taken from JamesM's kernel development tutorial.
 */

#ifndef KHEAP_H
#define KHEAP_H

#include "common.h"
#include "ordered-array.h"

/** Address where to put kernel heap. */
#define KHEAP_START         0xC0000000
/** Initial size of a heap. */
#define KHEAP_INIT_SIZE     0x100000
/** Size of heap index. */
#define HEAP_INDEX_SIZE    0x20000
/** Magic number to verify consistency of memory. */
#define HEAP_MAGIC          0x123890AB
/** Minimal size of a heap. Do not contract the heap if size would fall
 * below this limit. */
#define HEAP_MIN_SIZE       0x70000

/**
 * Header of heap section.
 */
typedef struct {
    /** Magic number, used for error checking and identification. */
    u32int magic;
    /** Type of section. 1 if this is a hole, 0 if this is a block. */
    u8int is_hole;
    /** Size of this block, including the end footer. */
    u32int size;
} header_t;

/**
 * Footer of heap section.
 */
typedef struct {
    /** Magic number, same as in header_t. */
    u32int magic;
    /** Pointer to the block header. */
    header_t *header;
} footer_t;

/*
 * These macros help simplify code that works with headers and footers.
 * They typecast their argument to pointer to a header_t or footer_t.
 */
/** Typecast `x` to header_t. */
#define HEADER_T(x) ((header_t *)(x))
/** Typecast `x` to footer_t. */
#define FOOTER_T(x) ((footer_t *)(x))

/**
 * Get pointer to a footer matching given header.
 */
#define HEADER_GET_FOOTER(h) FOOTER_T((u32int) h + h->size - sizeof(footer_t))

/**
 * The heap itself.
 */
typedef struct {
    /** Index of the heap. */
    ordered_array_t index;
    /** The start of our allocated space. */
    u32int start_addr;
    /** The end of our allocated space. May be expanded up to max_address. */
    u32int end_addr;
    /** The maximum address heap can be expanded to. */
    u32int max_addr;
    /** Should extra pages requested be mapped as supervisor-only? */
    u8int supervisor;
    /** Should extra pages requested be mapped as read-only? */
    u8int readonly;
} heap_t;

/**
 * Create a new heap.
 *
 * @param start         start address of the heap
 * @param end           end address of the heap
 * @param max           maximum address the heap can be expanded to
 * @param supervisor    whether to map extra pages requested as supervisor-only
 * @param readonly      whether to map extra pages requested as read-only
 * @return              newly created heap
 */
heap_t * heap_create(u32int start, u32int end, u32int max,
                     u8int supervisor, u8int readonly);

/**
 * Allocates a contiguous region of memory 'size' in size. If page_align
 * is set, the block will start on page boundary.
 *
 * @param heap          heap on which to allocate
 * @param size          how many bytes should by allocated
 * @param page_align    whether to page-align the memory
 * @return              pointer to newly allocated memory
 */
void * alloc(heap_t *heap, u32int size, u8int page_align);

/**
 * Releases a block allocated with alloc().
 *
 * @param heap  heap on which the block was allocated
 * @param p     pointer to memory to be freed
 */
void free(heap_t *heap, void *p);

/**
 * Allocate block of given size. If align is nonzero, page align it. If phys
 * is present, store physical address there.
 *
 * @param sz        requested size of the memory block
 * @param align     nonzero if the block is to be page-aligned
 * @param[out] phys where to store physical address [null]
 * @return          pointer to newly allocated memory
 */
void * kmalloc_internal(u32int sz, int align, u32int *phys);

/**
 * Allocate `sz` bytes of memory and return its address.
 * This is a wrapper around kmalloc_internal().
 *
 * @param sz        requested size of the memory block
 * @return          pointer to newly allocated memory
 */
void * kmalloc(u32int sz);

/**
 * Allocate page aligned memory of size `sz`.
 * This is a wrapper around kmalloc_internal().
 *
 * @param sz        requested size of the memory block
 * @return          pointer to newly allocated memory
 */
void * kmalloc_a(u32int sz);

/**
 * Allocate `sz` bytes and return physical address.
 * This is a wrapper around kmalloc_internal().
 *
 * @param sz        requested size of the memory block
 * @param[out] phys where to store physical address [null]
 * @return          pointer to newly allocated memory
 */
void * kmalloc_p(u32int sz, u32int *phys);

/**
 * Allocate page aligned block.
 * This is a wrapper around kmalloc_internal().
 *
 * @param sz        requested size of the memory block
 * @param[out] phys where to store physical address [null]
 * @return          pointer to newly allocated memory
 */
void * kmalloc_ap(u32int sz, u32int *phys);

/**
 * Free memory allocated with kmalloc after heap was enabled.
 *
 * @param p address which should be freed
 */
void kfree(u32int p);

#endif /* end of include guard: KHEAP_H */
