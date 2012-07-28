/*
 * ide.h -- Defines interface for and structures relating to IDE drives.
 */

#ifndef IDE_H
#define IDE_H

#include "common.h"

/*
 * Initialise the drive.
 *
 * @param bar0  base address of primary channel
 * @param bar1  base address of primary channel control port
 * @param bar2  base address of secondary channel
 * @param bar3  base address of secondary channel control port
 * @param bar4  bus Master IDE
 */
void initialise_ide(u32int bar0, u32int bar1,
        u32int bar2, u32int bar3, u32int bar4);

/* Read or write? */
typedef enum {
    ATA_READ    = 0x00,
    ATA_WRITE   = 0x01
} ata_direction_t;

/*
 * Read or write from ATA drive.
 *
 * @param direction read or write?
 * @param drive     the drive number from 0 to 3
 * @param lba       the LBA address
 * @param numsects  number of sectors to be read or written; if zero,
 *                  read 256 sectors at once
 * @param selector  the segment selector to read from or write to
 * @param offset    offset in that segment
 */
u8int ide_ata_access(ata_direction_t direction, u8int drive, u32int lba,
        u8int numsects, u16int selector, u8int edi);

#endif /* end of include guard: IDE_H */
