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

#endif /* end of include guard: COMMON_H */
