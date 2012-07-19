/*
 * main.c -- defines the C-code kernel entry point, calls the initialization
 * routines.
 * Copied from JamesM's tutorials.
 */

int main(struct multiboot *mboot_ptr)
{
    monitor_clear();
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
