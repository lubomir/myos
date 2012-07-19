/* link.ld -- Linker script for the kernel - ensure everything goes in the
 * correct place.
 * Original file taken from Bran's Kernel Development
 * This copy comes from JamesM's kernel tutorials.
 */

/* Start point is symbol 'start' */
ENTRY(start)
SECTIONS
{
	/* All sections will be page aligned. Page size is 1 KB. */

	/* Text section (where the code goes) should be first, should start
	 * at 0x100000 (1 MB) */
	.text 0x100000 :
	{
		code = .; _code = .; __code = .;
		*(.text)
		. = ALIGN(4096);
	}

	/* .data (initialized static data) should be next */
	.data :
	{
		data = .; _data = .; __data = .;
		*(.data)
		/* read-only initialized data (such as constants) */
		*(.rodata)
		. = ALIGN(4096);
	}

	/* .bss (uninitialized static data) should be last */
	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
		. = ALIGN(4096);
	}

	end = .; _end = .; __end = .;
}
