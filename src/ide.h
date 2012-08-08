/**
 * @file    ide.h
 *
 * Defines interface for and structures relating to IDE drives.
 */

#ifndef IDE_H
#define IDE_H

#include "common.h"

/**
 * Initialise the drive.
 */
void initialise_ide(void);

/** Read or write? */
typedef enum {
    ATA_READ    = 0x00,     /**< Read */
    ATA_WRITE   = 0x01      /**< Write */
} ata_direction_t;

/**
 * Read or write from ATA drive.
 *
 * @param direction read or write?
 * @param drive     the drive number from 0 to 3
 * @param lba       the LBA address
 * @param numsects  number of sectors to be read or written; if zero,
 *                  read 256 sectors at once
 * @param buf       buffer to write to or read from
 * @return 0 on success, non-zero on failure
 */
u8int ide_ata_access(ata_direction_t direction, u8int drive, u32int lba,
        u8int numsects, u16int *buf);

#endif /* end of include guard: IDE_H */
