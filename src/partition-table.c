/*
 * partition-table.c -- Defines function implementing reading of partition
 *                      table.
 */

#include <string.h>

#include "ide.h"
#include "kheap.h"
#include "partition-table.h"

pt_entry_t * partition_load(u8int drive)
{
    u8int buffer[512];
    pt_entry_t *table;

    table = kmalloc(4 * sizeof(pt_entry_t));
    ide_ata_access(ATA_READ, drive, 0, 1, (u16int *) buffer);


    memcpy(&table[0], buffer + PT1_OFFSET, sizeof(pt_entry_t));
    memcpy(&table[1], buffer + PT2_OFFSET, sizeof(pt_entry_t));
    memcpy(&table[2], buffer + PT3_OFFSET, sizeof(pt_entry_t));
    memcpy(&table[3], buffer + PT4_OFFSET, sizeof(pt_entry_t));

    return table;
}

