/*
 * kheap.c -- defines basic kernel heap
 */

#include "kheap.h"

/* end is defined in the linker script */
extern u32int end;
u32int placement_address = (u32int) &end;

u32int kmalloc_internal(u32int sz, int align, u32int *phys)
{
    /* If the address is not already aligned, align it. */
    if (align == 1 && (placement_address & 0xFFFFF000)) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
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
