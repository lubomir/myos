/*
 * kheap.h -- defines interface to basic kernel heap
 */

#ifndef KHEAP_H
#define KHEAP_H

#include "common.h"

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
