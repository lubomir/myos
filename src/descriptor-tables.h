/*
 * descriptor-table.c -- defines Global Descriptor Table and Interrubt
 * Descriptor Table.
 * From JamesM's kernel development tutorials.
 */

#ifndef DESCRIPTOR_TABLE_H
#define DESCRIPTOR_TABLE_H

#include "common.h"

/**
 * Access byte format:
 * | 7 | 6 5 |  4 | 3  0 |
 * | P | DPL | DT | Type |
 *
 * P    - is segment present? (1 -> Yes)
 * DPL  - descriptor privilege level - Ring 0-3
 * DT   - descriptor type
 * Type - segment type - code/data segment
 *
 * Granularity byte format:
 * | 7 | 6 | 5 | 4 | 3 0 |
 * | G | D | 0 | A | Len |
 *
 * G    - granularity (0 = 1 B, 1 = 1 kB)
 * D    - operand size (0 = 16 b, 1 = 32 b)
 * 0    - should always be zero
 * A    - available for system use (always zero)
 */

/*
 * This structure contains the value of one GDT entry. We use the attribute
 * 'packed' to tell GCC not to change any of the alignment in the structure.
 */
struct gdt_entry_struct {
    /* The lower 16 bits of the limit. */
    u16int limit_low;
    /* The lower 16 bits of the base. */
    u16int base_low;
    /* The next 8 bits of the base. */
    u8int base_middle;
    /* Access flags, determine what ring this segment can be used in. */
    u8int access;
    u8int granularity;
    /* The last 8 bits of the base. */
    u8int base_high;
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct {
    /* The upper 16 bits of all selector limits. */
    u16int limit;
    /* The address of the first gdt_entry_t struct */
    u32int base;
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

/*
 * Flags byte format:
 * | 7 | 6 5 | 4   0 |
 * | P | DPL | 00110 |
 *
 * P    - is entry present? (0 will cause "Interrupt Not Handled")
 * DPL  - privilege we expect to be called from
 * The lower 5 bits should always be 0b0110 - 14 in decimal
 */

/*
 * A struct describing an interrupt gate.
 */
struct idt_entry_struct {
    /* The lower 16 bits of the address to jump to when this interrupt fires */
    u16int base_lo;
    /* Kernel segment selector */
    u16int sel;
    /* This must always be zero */
    u8int always0;
    /* More flags. See docs. */
    u8int flags;
    /* The upper 16 bits of the address to jump to */
    u16int base_hi;
} __attribute__((packed));

typedef struct idt_entry_struct idt_entry_t;

/*
 * A struct describing a pointer to an array of interrupt handlers.
 * This is in a format suitable for giving to 'lidt'.
 */
struct idt_ptr_struct {
    u16int limit;
    /* The address of the first element in our idt_entry_t array. */
    u32int base;
} __attribute__((packed));

typedef struct idt_ptr_struct idt_ptr_t;

/*
 * These extern directives let us access the addresses of our ASM ISR handlers.
 */
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

/*
 * Initialisation function is publicly accessible.
 */
void init_descriptor_tables(void);

#endif /* end of include guard: DESCRIPTOR-TABLE_H */
