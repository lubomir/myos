/*
 * descriptor-tables.c -- initialises the GDT and IDT and defines the default
 *                        ISR and IRQ handler.
 * From JamesM's kernel development tutorials.
 */

#include <string.h>

#include "common.h"
#include "descriptor-tables.h"
#include "isr.h"

/* This lets us call ASM functions from the C code. */
extern void gdt_flush(u32int);
extern void idt_flush(u32int);
extern void tss_flush(void);

/* Internal functions */
static void init_gdt(void);
static void gdt_set_gate(s32int, u32int, u32int, u8int, u8int);
static void init_idt(void);
static void idt_set_gate(u8int, u32int, u16int, u8int);
static void write_tss(s32int, u16int, u32int);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;
tss_entry_t tss_entry;

extern isr_t interrupt_handlers[];

/*
 * Initialization routine zeroes all the interrupt service routines,
 * initialises GDT and IDT.
 */
void init_descriptor_tables(void)
{
    init_gdt();
    init_idt();
    memset(&interrupt_handlers, 0, sizeof (isr_t) * 256);
}

static void init_gdt(void)
{
    gdt_ptr.limit = (sizeof (gdt_entry_t) * 5) - 1;
    gdt_ptr.base  = (u32int) &gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                /* Null segment */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* Code segment */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* Data segment */
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* User mode code segment */
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* User mode data segment */
    write_tss(5, 0x10, 0x0);

    gdt_flush((u32int) &gdt_ptr);
    tss_flush();
}

/*
 * Initialise our task state segment structure.
 */
static void write_tss(s32int num, u16int ss0, u32int esp0)
{
    /* First, let's compute the base and limit of our entry into the GDT. */
    u32int base  = (u32int) &tss_entry;
    u32int limit = base + sizeof(tss_entry);

    /* Now, add our TSS descriptor's address to the GDT. */
    gdt_set_gate(num, base, limit, 0xE9, 0x00);

    /* Ensure the descriptor is initially zero. */
    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0  = ss0;       /* Set the kernel stack segment. */
    tss_entry.esp0 = esp0;      /* Set the kernel stack pointer. */

    /* Here we set the cs, ss, ds, es, fs and gs entries in the TSS. These
     * specify what segments should be loaded when the processor switches
     * to kernel mode. Therefore they are just our normal kernel code/data
     * segments - 0x08 and 0x10 respectively, but with the last two bits
     * set, making 0x0B and 0x13. The settings of htese bits sets the RPL
     * (Requested Privilege Level) to 3, meaning that this TSS can be used
     * to switch to kernel mode from ring 3. */
    tss_entry.cs = 0x0b;
    tss_entry.ss = tss_entry.ds =
        tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
}

void set_kernel_stack(u32int stack)
{
    tss_entry.esp0 = stack;
}

/*
 * Set the value of one GDT entry.
 */
static void gdt_set_gate(s32int num,
                         u32int base,
                         u32int limit,
                         u8int access,
                         u8int gran)
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

static void init_idt(void)
{
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base  = (u32int) &idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);

    /* Remap the irq table */
    outb(PIC1, ICW1_ICW4 | ICW1_INIT); /* Start init sequence */
    outb(PIC2, ICW1_ICW4 | ICW1_INIT);
    outb(PIC1_DATA, 0x20);  /* Master PIC vector starts at 0x20 (32) */
    outb(PIC2_DATA, 0x28);  /* Slave PIC vector starts at 0x28 (40) */
    outb(PIC1_DATA, 0x04);  /* Tell master that there is slave at IRQ2 */
    outb(PIC2_DATA, 0x02);  /* Tell slave its identity IRQ2 (0000 0100) */
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);

    idt_set_gate( 0, (u32int) isr0,  0x08, 0x8E);
    idt_set_gate( 1, (u32int) isr1,  0x08, 0x8E);
    idt_set_gate( 2, (u32int) isr2,  0x08, 0x8E);
    idt_set_gate( 3, (u32int) isr3,  0x08, 0x8E);
    idt_set_gate( 4, (u32int) isr4,  0x08, 0x8E);
    idt_set_gate( 5, (u32int) isr5,  0x08, 0x8E);
    idt_set_gate( 6, (u32int) isr6,  0x08, 0x8E);
    idt_set_gate( 7, (u32int) isr7,  0x08, 0x8E);
    idt_set_gate( 8, (u32int) isr8,  0x08, 0x8E);
    idt_set_gate( 9, (u32int) isr9,  0x08, 0x8E);
    idt_set_gate(10, (u32int) isr10, 0x08, 0x8E);
    idt_set_gate(11, (u32int) isr11, 0x08, 0x8E);
    idt_set_gate(12, (u32int) isr12, 0x08, 0x8E);
    idt_set_gate(13, (u32int) isr13, 0x08, 0x8E);
    idt_set_gate(14, (u32int) isr14, 0x08, 0x8E);
    idt_set_gate(15, (u32int) isr15, 0x08, 0x8E);
    idt_set_gate(16, (u32int) isr16, 0x08, 0x8E);
    idt_set_gate(17, (u32int) isr17, 0x08, 0x8E);
    idt_set_gate(18, (u32int) isr18, 0x08, 0x8E);
    idt_set_gate(19, (u32int) isr19, 0x08, 0x8E);
    idt_set_gate(20, (u32int) isr20, 0x08, 0x8E);
    idt_set_gate(21, (u32int) isr21, 0x08, 0x8E);
    idt_set_gate(22, (u32int) isr22, 0x08, 0x8E);
    idt_set_gate(23, (u32int) isr23, 0x08, 0x8E);
    idt_set_gate(24, (u32int) isr24, 0x08, 0x8E);
    idt_set_gate(25, (u32int) isr25, 0x08, 0x8E);
    idt_set_gate(26, (u32int) isr26, 0x08, 0x8E);
    idt_set_gate(27, (u32int) isr27, 0x08, 0x8E);
    idt_set_gate(28, (u32int) isr28, 0x08, 0x8E);
    idt_set_gate(29, (u32int) isr29, 0x08, 0x8E);
    idt_set_gate(30, (u32int) isr30, 0x08, 0x8E);
    idt_set_gate(31, (u32int) isr31, 0x08, 0x8E);

    idt_set_gate(32, (u32int) irq0,  0x08, 0x8E);
    idt_set_gate(33, (u32int) irq1,  0x08, 0x8E);
    idt_set_gate(34, (u32int) irq2,  0x08, 0x8E);
    idt_set_gate(35, (u32int) irq3,  0x08, 0x8E);
    idt_set_gate(36, (u32int) irq4,  0x08, 0x8E);
    idt_set_gate(37, (u32int) irq5,  0x08, 0x8E);
    idt_set_gate(38, (u32int) irq6,  0x08, 0x8E);
    idt_set_gate(39, (u32int) irq7,  0x08, 0x8E);
    idt_set_gate(40, (u32int) irq8,  0x08, 0x8E);
    idt_set_gate(41, (u32int) irq9,  0x08, 0x8E);
    idt_set_gate(42, (u32int) irq10, 0x08, 0x8E);
    idt_set_gate(43, (u32int) irq11, 0x08, 0x8E);
    idt_set_gate(44, (u32int) irq12, 0x08, 0x8E);
    idt_set_gate(45, (u32int) irq13, 0x08, 0x8E);
    idt_set_gate(46, (u32int) irq14, 0x08, 0x8E);
    idt_set_gate(47, (u32int) irq15, 0x08, 0x8E);
    idt_set_gate(128, (u32int) isr128, 0x08, 0x8E);

    idt_flush((u32int) &idt_ptr);
}

static void idt_set_gate(u8int num, u32int base, u16int sel, u8int flags)
{
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags | 0x60;
}
