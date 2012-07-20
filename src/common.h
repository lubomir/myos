/*
 * common.h -- defines typedefs and some global functions.
 * From JamesM's kernel development tutorials.
 */

#ifndef COMMON_H
#define COMMON_H

typedef unsigned int   u32int;
typedef          int   s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char  u8int;
typedef          char  s8int;

/*
 * Write a byte out to the specified port.
 */
void outb(u16int port, u8int value);

/*
 * Read a byte from specified port.
 */
u8int inb(u16int port);

/*
 * Read a two-byte word from specified port.
 */
u16int inw(u16int port);

/* Master PIC IO port */
#define PIC1            0x20
#define PIC1_DATA       (PIC1 + 1)
/* Slave PIC IO port */
#define PIC2            0xA0
#define PIC2_DATA       (PIC2 + 1)

#define EOI             0x20

#define ICW1_ICW4       0x01    /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08    /* Level triggered (edge) mode) */
#define ICW1_INIT       0x10    /* Initialization - required! */

#define ICW4_8086       0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02    /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW5_BUF_MASTER 0x0C    /* BUffered mode/master */
#define ICW_SFNM        0x10    /* Special fully nested (not) */

/*
 * This function prints error message and enters infinite loop.
 */
void panic(const char *msg, const char *file, int line)
    __attribute__((noreturn));

/*
 * Accessor macro for panic() function that fills in filename and line number
 * automatically.
 */
#define PANIC(msg) do { panic(msg, __FILE__, __LINE__); } while (0)

#endif /* end of include guard: COMMON_H */
