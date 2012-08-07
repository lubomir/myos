/**
 * @file    descriptor-tables.h
 *
 * Defines Global Descriptor Table and Interrupt Descriptor Table.
 *
 * From JamesM's kernel development tutorials.
 */

#ifndef DESCRIPTOR_TABLES_H
#define DESCRIPTOR_TABLES_H

#include "common.h"

/**
 * Initialisation function is publicly accessible.
 */
void init_descriptor_tables(void);

/**
 * Set kernel stack pointer.
 *
 * @param stack new value for kernel stack pointer (esp)
 */
void set_kernel_stack(u32int stack);

#endif /* end of include guard: DESCRIPTOR_TABLES_H */
