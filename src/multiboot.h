/**
 * @file    multiboot.h
 *
 * Defines multiboot header. This header is passed to kernel by GRUB
 * (or other boot manager).
 *
 * Taken from JamesM's kernel development tutorial.
 */
#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include "common.h"

#define MULTIBOOT_FLAG_MEM      0x001
#define MULTIBOOT_FLAG_DEVICE   0x002
#define MULTIBOOT_FLAG_CMDLINE  0x004
#define MULTIBOOT_FLAG_MODS     0x008
#define MULTIBOOT_FLAG_AOUT     0x010
#define MULTIBOOT_FLAG_ELF      0x020
#define MULTIBOOT_FLAG_MMAP     0x040
#define MULTIBOOT_FLAG_CONFIG   0x080
#define MULTIBOOT_FLAG_LOADER   0x100
#define MULTIBOOT_FLAG_APM      0x200
#define MULTIBOOT_FLAG_VBE      0x400

/** Structure for Multiboot header. */
struct multiboot {
    /** Indicates presence and validity of other fields. */
    u32int flags;

    /**
     * Amount of lower memory. Maximum possible value is 640 kilobytes.
     *
     * Present if `flags[0]` is set.
     */
    u32int mem_lower;

    /**
     * Amount of upper memory.
     *
     * Present if `flags[0]` is set.
     */
    u32int mem_upper;

    /**
     * Which BIOS disk device was the system loaded from. This field is laid
     * out in four one-byte subfields as follows:
     *
     *      | part3 | part2 | part1 | drive |
     *
     *  The first byte contains the BIOS drive number as understood by the
     *  BIOS INT 0x13 low-level disk interface: e.g. 0x00 for the  first
     *  floppy disk or 0x80 for the first hard disk.
     *
     *  The three remaining bytes specify the boot partition. `part1`
     *  specifies the top-level partition number, `part2` specifies a
     *  sub-partition in top-level partition, etc. Particular numbers always
     *  start from zero. Unused partition bytes must be set to 0xFF.
     *
     *  Present if `flags[1]` is set.
     */
    u32int boot_device;

    /**
     * Contains the physical address of the command line to be passed to the
     * kernel. The command line is a normal C-style zero terminated string.
     *
     * Present if `flags[2]` is set. */
    u32int cmdline;


    /**
     * Contains the number of modules loaded. `mods_count` may be zero, even
     * if flags[3] is set (there just are no loaded modules).
     *
     * Present if `flags[3]` is set.
     */
    u32int mods_count;

    /**
     * Contains the physical address of the first module structure.
     *
     * Each module structure is formatted as follows:
     *
     *      0  | mod_start    |
     *      4  | mod_end      |
     *      8  | string       |
     *      12 | reserved (0) |
     *
     * The first two field contain the start and end addresses of the boot
     * module itself. The `string` field provides an arbitrary string to be
     * associated with that particular module. It is a zero-terminated ASCII
     * string, just like `cmdline`. The `string` field may be 0 if there is no
     * string associated with the module. The reserved field must contain 0
     * and be ignored.
     *
     * Present if `flags[3]` is set.
     */
    u32int mods_addr;


    /**
     * Corresponds to `shdr_num` in ELF header.
     *
     * This field may be 0, indicating no symbols, even if bit 5 in the
     * `flags` word is set.
     *
     * Present if `flags[5]` is set.
     */
    u32int num;

    /**
     * Corresponds to `shdr_size` in ELF header.
     *
     * Present if `flags[5]` is set.
     */
    u32int size;

    /**
     * Corresponds to `shdr_addr` in ELF header.
     *
     * Present if `flags[5]` is set.
     */
    u32int addr;

    /**
     * Corresponds to `shdr_shndx` in ELF header.
     *
     * Present if `flags[5]` is set.
     */
    u32int shndx;


    /**
     * Indicate length of a buffer containing a memory map of the machine
     * provided by BIOS.
     *
     * The buffer consists of one or more of the following size/structure
     * pairs (size is really used for skipping to the next pair).
     *
     *      -4  | size      |
     *          +-----------+
     *      0   | base_addr |
     *      8   | length    |
     *      16  | type      |
     *
     * Where `size` is the size of the associated structure in bytes, which
     * can be greater than the minimum of 20 bytes. `base_addr` is the
     * starting address, `length` is the size of the memory region in bytes,
     * `type` is the variety of address range represented, where a value of 1
     * indicates available RAM, and all other values currently indicate a
     * reserved area.
     *
     * Present if `flags[6]` is set.
     */
    u32int mmap_length;

    /**
     * Indicate address of a buffer containing a memory map of the machine
     * provided by BIOS.
     *
     * Present if `flags[6]` is set.
     */
    u32int mmap_addr;


    /**
     * Size of the drive structures. It may be zero.
     *
     * Present if `flags[7]` is set.
     */
    u32int drives_length;

    /**
     * Address of the first drive structure.
     *
     * Each drive structure is formatted as follows:
     *
     *      0       | size              |
     *              +-------------------+
     *      4       | drive_number      |
     *      5       | drive_mode        |
     *              +-------------------+
     *      6       | drive_cylinders   |
     *      8       | drive_heads       |
     *      9       | drive_sectors     |
     *              +-------------------+
     *      10 - xx | drive_ports       |
     *
     * The `size` field specifies the size of this structure. The size varies,
     * depending on the number of ports. Note that the size my not be equal
     * to (10 + 2 * number of the ports), because of an alignment.
     *
     * The `drive_number` field contains the BIOS drive number. The
     * `drive_mode` field represents the access mode used by the boot loader.
     * Currently, the following modes are defined: 0 (CHS mode), 1 (LBA mode).
     *
     * Next three fields indicate the geometry of the drive detected by BIOS.
     *
     * The `drive_ports` field contains an array of the IO ports used for
     * the drive in the BIOS code. The array contains of zero or more unsigned
     * two-byte integers, and is terminated with zero. Node that the array may
     * contain any number of IO ports that are not related to the drive
     * actually (such as DMA controller's ports).
     *
     * Present if `flags[7]` is set.
     */
    u32int drives_addr;


    /**
     * Indicates the address of the ROM configuration table. The table is
     * returned by *GET CONFIGURATION* BIOS call. If the BIOS call fails,
     * then the size of the table must be zero.
     *
     * Present if `flags[8]` is set.
     */
    u32int config_table;

    /**
     * Contains the physical address of the name of a boot loader. The name
     * is a normal C-style zero-terminated string.
     *
     * Present if `flags[9]` is set.
     */
    u32int boot_loader_name;

    /**
     * Contains the physical address of an APM table. The table is defined
     * as below:
     *
     *      0   | version       |   version number
     *      2   | cseg          |   protected mode 32-bit code segment
     *      4   | offset        |   offset of the entry point
     *      8   | cseg_16       |   protected mode 16-bit code segment
     *      10  | dseg          |   protected mode 16-bit data segment
     *      12  | flags         |   the flags
     *      14  | cseg_len      |   length of protected mode 32b code segment
     *      16  | cseg_16_len   |   length of protected mode 16b code segment
     *      18  | dseg_len      |   length of protected mode 16b data segment
     *
     * Only the field `offset` is 4 bytes, and the others are 2 bytes.
     *
     * Present if `flags[10]` is set.
     */
    u32int apm_table;


    /**
     * Contains the physical address of VBE control information returned by
     * the VBE function 00h.
     *
     * Present if `flags[11]` is set.
     */
    u32int vbe_control_info;

    /**
     * Contains the physical address of VBE mode information returned by the
     * VBE function 01h.
     *
     * Present if `flags[11]` is set.
     */
    u32int vbe_mode_info;

    /**
     * Indicates current video mode. The format is specified in VBE 3.0.
     *
     * Present if `flags[11]` is set.
     */
    u32int vbe_mode;

    /**
     * Contains the table of a protected mode interface defined in VBE 2.0+.
     *
     * Present if `flags[11]` is set.
     */
    u32int vbe_interface_seg;

    /**
     * Contains the table of a protected mode interface defined in VBE 2.0+.
     *
     * Present if `flags[11]` is set.
     */
    u32int vbe_interface_off;

    /**
     * Contains the table of a protected mode interface defined in VBE 2.0+.
     *
     * Present if `flags[11]` is set.
     */
    u32int vbe_interface_len;
} PACKED;

#endif /* end of include guard: MULTIBOOT_H */
