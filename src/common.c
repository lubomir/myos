/*
 * common.c -- defines some global functions.
 * From JamesM's kernel development tutorials.
 */

#include "common.h"
#include "monitor.h"

void outb(u16int port, u8int value)
{
    asm volatile ("outb %1, %0" :: "dN" (port), "a" (value));
}

u8int inb(u16int port)
{
    u8int ret;
    asm volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

u16int inw(u16int port)
{
    u16int ret;
    asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

void * memset(u8int *dest, u8int val, u32int len)
{
    u8int *tmp = dest;
    for ( ; len > 0; --len) *tmp++ = val;
    return dest;
}

void * memcpy(u8int *dest, const u8int *src, u32int len)
{
    u8int *tmp = dest;
    for ( ; len > 0; --len) *tmp++ = *src++;
    return dest;
}

int strcmp(const char *str1, const char *str2)
{
    while (*str1 && *str2) {
        if (*str1 < *str2) {
            return -1;
        }
        if (*str1 > *str2) {
            return 1;
        }
        str1++;
        str2++;
    }
    if (!*str1 && *str2) {
        return -1; /* str1 is longer */
    }
    if (*str1 && !*str2) {
        return 1; /* str2 is longer */
    }
    return 0;
}

char * strcpy(char *dest, const char *src)
{
    char *dp = dest;
    do {
        *dp++ = *src++;
    } while (*src);
    return dest;
}

char * strcat(char *dest, const char *src)
{
    char *dp = dest;
    while (*dp) {
        dp++;
    }

    do {
        *dp++ = *src++;
    } while (*src);

    return dest;
}

void panic(const char *msg, const char *file, int line)
{
    /* Disable interrupts. */
    asm volatile ("cli");

    monitor_print("---\nKernel panic at %s:%u\n%s", file, line, msg);
    while (1);
}
