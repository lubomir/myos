/*
 * partition-table.h -- Defines interface for and structures relating to
 *                      reading partition table from hard drives.
 */

#ifndef UNTITLED_H
#define UNTITLED_H

#include "common.h"

#define PT1_OFFSET  0x01BE
#define PT2_OFFSET  0x01CE
#define PT3_OFFSET  0x01DE
#define PT4_OFFSET  0x01EE

#define SYS_EMPTY   0x00
#define SYS_FAT12   0x01
#define SYS_SWAP    0x82
#define SYS_LINUX   0x83

#define PT_ACTIVE   0x80
#define PT_INACTIVE 0x00

struct pt_entry {
    /* 0x80 = bootable (active), 0x0 = non-bootable. */
    u8int boot;
    /* Starting head. */
    u8int shead;
    /* Start sector. */
    u16int ssec : 6;
    /* Starting cylinder. */
    u16int scyl : 10;
    /* System ID. */
    u8int sysid;
    /* Ending head. */
    u8int ehead;
    /* Ending sector. */
    u16int esec : 6;
    /* Ending cylinder. */
    u16int ecyl : 10;
    /* Relative sector to start of partition - this is partition's starting
     * LBA value. */
    u32int relsec;
    /* Total number of sectors in a partition. */
    u32int numsec;
} PACKED;

typedef struct pt_entry pt_entry_t;

/*
 * Load a partition table from specified drive. Returns newly allocated array
 * of four pt_entry_t's. All unused entries are all zeroed.
 */
pt_entry_t * partition_load(u8int drive);

#endif /* end of include guard: UNTITLED_H */
