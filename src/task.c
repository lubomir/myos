/*
 * task.c -- Defines the functions needed to multitask.
 * Written for JamesM's kernel development tutorial.
 */

#include <string.h>

#include "paging.h"
#include "task.h"

/* Defined in kmain.c */
extern u32int initial_esp;

/* Defined in paging.c */
extern page_directory_t *current_directory;

void move_stack(void *new_stack_start, u32int size)
{
    u32int i;
    /* Allocate some space for the new stack. */
    for (i = (u32int) new_stack_start;
            i >= (u32int)new_stack_start - size;
            i -= 0x1000) {
        /* General purpose stack is in user-mode. */
        alloc_frame(get_page(i, 1, current_directory),
                0 /* User-mode */, 1 /* writable */);
    }

    /* Flush the TLB by reading and writing the page directory address again. */
    u32int pd_addr;
    asm volatile ("mov %%cr3, %0" : "=r" (pd_addr));
    asm volatile ("mov %0, %%cr3" :: "r" (pd_addr));

    /* Old ESP and EBP, read from registers. */
    u32int old_stack_pointer, old_base_pointer;
    asm volatile("mov %%esp, %0" : "=r" (old_stack_pointer));
    asm volatile("mov %%ebp, %0" : "=r" (old_base_pointer));

    /* Offset to add to old stack addresses to get a new stack address. */
    u32int offset = (u32int) new_stack_start - initial_esp;

    /* New ESP and EBP. */
    u32int new_stack_pointer = old_stack_pointer + offset;
    u32int new_base_pointer  = old_base_pointer  + offset;

    /* Copy the stack. */
    memcpy((void *) new_stack_pointer, (void *) old_stack_pointer,
            initial_esp - old_stack_pointer);

    /* Backtrace through the original stack, copying new values
     * the new stack. */
    for (i = (u32int) new_stack_start;
            i > (u32int) new_stack_start - size;
            i -= 4) {
        u32int tmp = * (u32int *) i;
        /* If the value of tmp is inside the range of the old stack, assume
         * it is a base pointer and remap it. This will unfortunately remap
         * ANY value in this range, whether they are base pointers or not. */
        if (old_stack_pointer < tmp && tmp < initial_esp) {
            tmp = tmp + offset;
            u32int *tmp2 = (u32int *) i;
            *tmp2 = tmp;
        }
    }

    asm volatile ("mov %0, %%esp" : : "r" (new_stack_pointer));
    asm volatile ("mov %0, %%ebp" : : "r" (new_base_pointer));
}
