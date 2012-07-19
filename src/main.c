/*
 * main.c -- defines the C-code kernel entry point, calls the initialization
 * routines.
 * Copied from JamesM's tutorials.
 */

#include "common.h"

#define CHECK_INT(TYPE, SIZE) do {              \
    if (sizeof(TYPE) != SIZE) {                 \
        monitor_write("PROBLEM: ");             \
        monitor_write(#TYPE " has size ");      \
        monitor_write_dec(sizeof(TYPE));        \
        monitor_write(", "#SIZE" expected\n");  \
    } } while (0)

static void check_types(void)
{
    monitor_write("Checking types...\n");
    CHECK_INT(u32int, 4);
    CHECK_INT(u16int, 2);
    CHECK_INT(u8int, 1);
    CHECK_INT(s32int, 4);
    CHECK_INT(s16int, 2);
    CHECK_INT(s8int, 1);
}

int main(struct multiboot *mboot_ptr)
{
    monitor_clear();
    check_types();
    monitor_write("Hello, world!\n");
    monitor_write("Decimal number: ");
    monitor_write_dec(12345);
    monitor_write(" ");
    monitor_write_dec(0);
    monitor_write(" ");
    monitor_write_dec(10101010);
    monitor_write("\nHexa number: ");
    monitor_write_hex(0xDEADCAFE);
    monitor_write(" ");
    monitor_write_hex(0x0);
    monitor_write(" ");
    monitor_write_hex(0x10101010);
    monitor_write("\n");
    return 0xDEADBABA;
}
