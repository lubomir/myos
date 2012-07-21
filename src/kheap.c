/*
 * kheap.c -- defines basic kernel heap
 */

#include "kheap.h"
#include "paging.h"

/*
 * Check if address is page aligned.
 */
#define PAGE_ALIGNED(addr) (((addr) & 0xFFFFF000) == 0)
/*
 * Page align given address.
 * Please note that this macro modifies its parameter, so you can only pass
 * it expressions that are proper left hand side of assignment.
 */
#define PAGE_ALIGN(addr) do {   \
        addr &= 0xFFFFF000;     \
        addr += 0x1000;         \
    } while (0)

/* This is the main kernel heap. */
heap_t *kheap = 0;

/* end is defined in the linker script */
extern u32int end;
u32int placement_address = (u32int) &end;

/* defined in paging.c */
extern page_directory_t *kernel_directory;

u32int kmalloc_internal(u32int sz, int align, u32int *phys)
{
    if (kheap) {
        void *addr = alloc(kheap, sz, (u8int)align);
        if (phys) {
            page_t *page = get_page((u32int) addr, 0, kernel_directory);
            *phys = page->frame * 0x1000 + ((u32int) addr & 0xFFF);
        }
        return (u32int) addr;
    }
    /* If the address is not already aligned, align it. */
    if (align == 1 && !PAGE_ALIGNED(placement_address)) {
        PAGE_ALIGN(placement_address);
    }
    if (phys) {
        *phys = placement_address;
    }
    u32int tmp = placement_address;
    placement_address += sz;
    return tmp;
}

u32int kmalloc(u32int sz)
{
    return kmalloc_internal(sz, 0, 0);
}

u32int kmalloc_a(u32int sz)
{
    return kmalloc_internal(sz, 1, 0);
}

u32int kmalloc_p(u32int sz, u32int *phys)
{
    return kmalloc_internal(sz, 0, phys);
}

u32int kmalloc_ap(u32int sz, u32int *phys)
{
    return kmalloc_internal(sz, 1, phys);
}

void kfree(u32int p)
{
    free(kheap, (void *)p);
}

/*
 * Find the smallest hole that will fit the given number of bytes.
 * Return -1 if no such block exists.
 */
static s32int find_smallest_hole(heap_t *heap, u32int size, u8int page_align)
{
    u32int iter = 0;

    while (iter < heap->index.size) {
        header_t *header = HEADER_T(oa_lookup(&heap->index, iter));
        if (page_align > 0) {   /* If the user requested page-aligned block */
            /* Page-align the starting point of this header. */
            u32int location = (u32int) header;
            s32int offset = 0;
            if (!PAGE_ALIGNED(location + sizeof (header_t)))
                offset = 0x1000 - (location + sizeof(header_t)) % 0x1000;
            s32int hole_size = (s32int) header->size - offset;
            /* Can we fit now? */
            if (hole_size >= (s32int) size)
                break;
        } else if (header->size >= size) {
            break;
        }
        iter++;
    }
    /* Why did the loop exit? */
    return iter == heap->index.size ? -1 /* Not found */ : iter;
}

/*
 * Compare two headers by their size.
 */
static int header_compare(void *a, void *b)
{
    return HEADER_T(a)->size - HEADER_T(b)->size;
}

heap_t * heap_create(u32int start, u32int end, u32int max,
                     u8int supervisor, u8int readonly)
{
    heap_t *heap = (heap_t *) kmalloc(sizeof(heap_t));

    /* All assumptions are made on startAddress and endAddress being
     * page-aligned. */
    ASSERT(start % 0x1000 == 0);
    ASSERT(end % 0x1000 == 0);

    /* Initialize the index. */
    heap->index = oa_place((void *) start, HEAP_INDEX_SIZE, &header_compare);

    /* Shift the start address forward to resemble where we can start putting
     * data. */
    start += sizeof(type_t) * HEAP_INDEX_SIZE;

    /* Make sure the start address is page aligned now. */
    if (!PAGE_ALIGNED(start)) {
        PAGE_ALIGN(start);
    }

    /* Write the start, end and max addresses into the heap structure. */
    heap->start_addr = start;
    heap->end_addr   = end;
    heap->max_addr   = max;
    heap->supervisor = supervisor;
    heap->readonly   = readonly;

    /* We start off with one large hole in the index. */
    header_t *hole = HEADER_T(start);
    hole->size = end - start;
    hole->magic = HEAP_MAGIC;
    hole->is_hole = 1;
    oa_insert(&heap->index, hole);

    return heap;
}

static void expand(heap_t *heap, u32int new_size)
{
    ASSERT(new_size > heap->end_addr - heap->start_addr);

    /* Get the nearest following page boundary. */
    if (!PAGE_ALIGNED(new_size)) {
        PAGE_ALIGN(new_size);
    }
    /* Make sure we are not overreaching ourselves. */
    ASSERT(heap->start_addr + new_size <= heap->max_addr);

    /* This should always be on page boundary. */
    u32int old_size = heap->end_addr - heap->start_addr;
    u32int i = old_size;
    while (i < new_size) {
        alloc_frame(get_page(heap->start_addr + i, 1, kernel_directory),
                heap->supervisor ? 1 : 0, heap->readonly ? 0 : 1);
        i += 0x1000;
    }
    heap->end_addr = heap->start_addr + new_size;
}

static u32int contract(heap_t *heap, u32int new_size)
{
    ASSERT(new_size < heap->end_addr - heap->start_addr);

    if (!PAGE_ALIGNED(new_size)) {
        PAGE_ALIGN(new_size);
    }

    /* Don't contract too far. */
    if (new_size < HEAP_MIN_SIZE)
        new_size = HEAP_MIN_SIZE;

    u32int old_size = heap->end_addr - heap->start_addr;
    u32int i = old_size - 0x1000;
    while (new_size < i) {
        free_frame(get_page(heap->start_addr + i, 0, kernel_directory));
        i -= 0x1000;
    }
    heap->end_addr = heap->start_addr + new_size;
    return new_size;
}

void * alloc(heap_t *heap, u32int size, u8int page_align)
{
    /* Make sure we take the size of header/footer into account. */
    u32int new_size = size + sizeof(header_t) + sizeof(footer_t);
    /* Smallest fitting hole. */
    s32int iter = find_smallest_hole(heap, new_size, page_align);

    if (iter == -1) {
        /* Save some previous data. */
        u32int old_length = heap->end_addr - heap->start_addr;
        u32int old_end_addr = heap->end_addr;

        /* We need to allocate some more space. */
        expand(heap, old_length + new_size);
        u32int new_length = heap->end_addr - heap->start_addr;

        /* Find the endmost header. (Not in size, but in location.) */
        iter = 0;
        /* Vars to hold the index of, and value of, the endmost header found
         * so far. */
        u32int idx = -1, value = 0;
        while (iter < heap->index.size) {
            u32int tmp = (u32int) oa_lookup(&heap->index, iter);
            if (tmp > value) {
                value = tmp;
                idx = iter;
            }
            iter++;
        }

        /* If we didn't find ANY headers, we need to add one. */
        if (idx == -1) {
            header_t *head = HEADER_T(old_end_addr);
            head->magic = HEAP_MAGIC;
            head->size  = new_length - old_length;
            head->is_hole = 1;
            footer_t *foot = FOOTER_T(old_end_addr + head->size - sizeof(footer_t));
            foot->magic = HEAP_MAGIC;
            foot->header = head;
            oa_insert(&heap->index, head);
        } else {
            /* The last header needs adjusting. */
            header_t *head = oa_lookup(&heap->index, idx);
            head->size += new_length - old_length;
            /* Rewrite the footer. */
            footer_t *foot = FOOTER_T((u32int)head + head->size - sizeof(footer_t));
            foot->header = head;
            foot->magic = HEAP_MAGIC;
        }
        /* We now have enough space. Recurse, and call the function again. */
        return alloc(heap, size, page_align);
    }

    header_t *orig_hole_header = HEADER_T(oa_lookup(&heap->index, iter));
    u32int orig_hole_pos = (u32int) orig_hole_header;
    u32int orig_hole_size = orig_hole_header->size;

    /* Here we work out if we should split the hole we found into two parts.
     * Is the original hole size - requested hole size less than the
     * overhead for adding new hole? */
    if (orig_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)) {
        /* Just increase the requested size to the size of the found hole. */
        size += orig_hole_size - new_size;
        new_size = orig_hole_size;
    }

    /* If we need to page-align the data, do it now and make a new hole in
     * front of our block. */
    if (page_align && !PAGE_ALIGNED(orig_hole_pos)) {
        u32int new_loc = orig_hole_pos + 0x1000
                                - (orig_hole_pos & 0xFFF)
                                - sizeof(header_t);
        header_t *header = HEADER_T(orig_hole_pos);
        header->size = 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);
        header->magic = HEAP_MAGIC;
        header->is_hole = 1;
        footer_t *footer = FOOTER_T((u32int) new_loc - sizeof(footer_t));
        footer->magic = HEAP_MAGIC;
        footer->header = header;
        orig_hole_pos = new_loc;
        orig_hole_size = orig_hole_size - header->size;
    } else {
        /* We don't need this hole anymore, delete it from index. */
        oa_remove(&heap->index, iter);
    }

    /* Overwrite the original header ... */
    header_t *block_head = HEADER_T(orig_hole_pos);
    block_head->magic   = HEAP_MAGIC;
    block_head->is_hole = 0;
    block_head->size    = new_size;
    /* ... and footer. */
    footer_t *block_foot = FOOTER_T(orig_hole_pos + size + sizeof(header_t));
    block_foot->magic   = HEAP_MAGIC;
    block_foot->header  = block_head;

    /* We may need to write a new hole after the allocated block. We do this
     * only if the new hole would have positive size. */
    if (orig_hole_size > new_size) {
        header_t *header = HEADER_T(orig_hole_pos + sizeof(header_t)
                                    + size + sizeof(footer_t));
        header->magic   = HEAP_MAGIC;
        header->is_hole = 1;
        header->size    = orig_hole_size - new_size;
        footer_t *footer = FOOTER_T((u32int)header + orig_hole_size
                                        - new_size - sizeof(footer_t));
        if ((u32int)footer < heap->end_addr) {
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        }
        oa_insert(&heap->index, header);
    }

    /* And we are done! */
    return (void *) ((u32int) block_head + sizeof(header_t));
}

void free(heap_t *heap, void *p)
{
    /* Exit gracefully for null pointers. */
    if (!p) {
        return;
    }

    /* Get the header and footer associated with this pointer. */
    header_t *header = HEADER_T((u32int)p - sizeof(header_t));
    footer_t *footer = HEADER_GET_FOOTER(header);

    /* Sanity check. */
    ASSERT(header->magic == HEAP_MAGIC);
    ASSERT(footer->magic == HEAP_MAGIC);

    /* Make us a hole. */
    header->is_hole = 1;

    /* Do we want to add this header into the free holes index? */
    char do_add = 1;

    /* Unify left.
     * If the thing immediately to the left of us is a footer ... */
    footer_t *test_f = FOOTER_T((u32int) header - sizeof(footer_t));
    if (test_f->magic == HEAP_MAGIC && test_f->header->is_hole) {
        u32int cache = header->size;    /* Cache our current size. */
        header = test_f->header;        /* Rewrite our header with new one. */
        footer->header = header;        /* Update header pointer. */
        header->size += cache;          /* Change the size. */
        do_add = 0;     /* This header is in the index, don't add anything. */
    }

    /* Unify right.
     * If the thing immediately to the right of us is a header ... */
    header_t *test_h = HEADER_T((u32int)footer + sizeof(footer_t));
    if (test_h->magic == HEAP_MAGIC && test_h->is_hole) {
        header->size += test_h->size;   /* Increase our size. */
        /* Find its footer. */
        test_f = FOOTER_T((u32int)test_h + test_h->size - sizeof(footer_t));
        test_f->header = header;
        footer = test_f;

        u8int removed = oa_remove_item(&heap->index, test_h);
        ASSERT(removed >= 0);
    }

    /* If the footer location is the end address, we can contract. */
    if ((u32int)footer + sizeof(footer_t) == heap->end_addr) {
        u32int old_length = heap->end_addr - heap->start_addr;
        u32int new_length = contract(heap, (u32int)header - heap->start_addr);
        /* Check how big we will be after resizing. */
        if (header->size > old_length - new_length) {
            header->size -= old_length - new_length;
            footer = HEADER_GET_FOOTER(header);
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        } else {
            oa_remove_item(&heap->index, test_h);
        }
    }

    if (do_add)
        oa_insert(&heap->index, header);
}
