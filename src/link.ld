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


	. = 0x00100000;

	/* This makes sure MultiBoot header is correctly places.
	 * Without this section GRUB might fail with Error 13 */
	.__mbHeader : {
		*(.__mbHeader)
	}

	/* Text section (where the code goes) should be first. */
	.text :
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
