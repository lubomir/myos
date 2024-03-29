/**
 * @file    common.h
 *
 * Defines types and some global functions.
 *
 * From JamesM's kernel development tutorials.
 */

#ifndef COMMON_H
#define COMMON_H

/** 32-bit unsigned int */
typedef unsigned int   u32int;
/** 32-bit signed int */
typedef          int   s32int;
/** 16-bit unsigned int */
typedef unsigned short u16int;
/** 16-bit signed int */
typedef          short s16int;
/** 8-bit unsigned int */
typedef unsigned char  u8int;
/** 8-bit signed int */
typedef          char  s8int;

/**
 * Write a byte out to the specified port.
 *
 * @param port  port to write to
 * @param value vale to be written
 */
void outb(u16int port, u8int value);

/**
 * Read a byte from specified port.
 *
 * @param port  port to read from
 * @return read value
 */
u8int inb(u16int port);

/**
 * Read a two-byte word from specified port.
 *
 * @param port  port to read from
 * @return read value
 */
u16int inw(u16int port);

/* Doxygen does not like the __attribute__ syntax, so hide it from it. */
#ifndef DOXYGEN_RUNNING
#define PACKED          __attribute__((packed))
#define NORETURN        __attribute__((noreturn))
#define FORMAT(a,b,c)   __attribute__((format(a,b,c)))
#else
#define PACKED          /**< Defines a structure as packed */
#define NORETURN        /**< Defines a function as non-returning */
#define FORMAT(a,b,c)   /**< Defines a function as printf-like */
#endif /* DOXYGEN_RUNNING */

#define PIC1            0x20        /**< Master PIC IO port */
#define PIC1_DATA       (PIC1+1)    /**< Master PIC data port */
#define PIC2            0xA0        /**< Slave PIC IO port */
#define PIC2_DATA       (PIC2+1)    /**< Slave PIC data port */

#define EOI             0x20        /**< End-Of-Interrupt code */

#define ICW1_ICW4       0x01        /**< ICW4 (not) needed */
#define ICW1_SINGLE     0x02        /**< Single (cascade) mode */
#define ICW1_INTERVAL4  0x04        /**< Call address interval 4 (8) */
#define ICW1_LEVEL      0x08        /**< Level triggered (edge) mode) */
#define ICW1_INIT       0x10        /**< Initialization - required! */

#define ICW4_8086       0x01        /**< 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02        /**< Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08        /**< Buffered mode/slave */
#define ICW5_BUF_MASTER 0x0C        /**< BUffered mode/master */
#define ICW_SFNM        0x10        /**< Special fully nested (not) */

/**
 * This function prints error message and enters infinite loop.
 *
 * @param msg   message to be printed
 * @param file  filename where panic occurred
 * @param line  line number where panic occured
 */
void panic(const char *msg, const char *file, int line) NORETURN;

/**
 * Accessor macro for panic() function that fills in filename and line
 * number automatically
 */
#define PANIC(msg) do { panic(msg, __FILE__, __LINE__); } while (0)

/**
 * Check that condition holds and panic if it does not
 */
#define ASSERT(exp) \
        do { if (!(exp)) PANIC("Assertion failed: "#exp); } while (0)

#endif /* end of include guard: COMMON_H */
