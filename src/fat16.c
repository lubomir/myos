/*
 */

#include <string.h>

#include "fat16.h"
#include "ide.h"
#include "kheap.h"
#include "monitor.h"

u8int buffer[4096];

void fat_setup(u8int drive, pt_entry_t *partition)
{
    ide_ata_access(ATA_READ, drive, partition->relsec, 1, (u16int *) buffer);
    fat_bs_t fat_boot;
    memcpy(&fat_boot, buffer, sizeof fat_boot);

    u32int root_dir_sectors, first_data_sector, first_fat_sector,
           data_sectors, total_clusters, type;

    root_dir_sectors =
        (fat_boot.root_entry_count * 32 + fat_boot.bytes_per_sector - 1)
        / fat_boot.bytes_per_sector;
    first_data_sector =
        fat_boot.reserved_sector_count +
        fat_boot.table_count * fat_boot.table_size16;
    first_fat_sector = fat_boot.reserved_sector_count;
    data_sectors =
        fat_boot.total_sectors_16 -
        (fat_boot.reserved_sector_count + root_dir_sectors +
         fat_boot.table_count * fat_boot.table_size16);
    total_clusters = data_sectors / fat_boot.sectors_per_cluster;

    if (total_clusters < 4085)
        type = 12;
    else if (total_clusters < 65525)
        type = 16;
    else
        type = 32;

    monitor_print("Type: FAT%u\n", type);

    fat_dir_t *dir = kmalloc(root_dir_sectors * fat_boot.bytes_per_sector);
    ide_ata_access(ATA_READ, drive, first_data_sector + partition->relsec,
            root_dir_sectors, (u16int *) dir);

    int j;
    for (j = 0; j < fat_boot.root_entry_count; ++j) {
        if (dir[j].name[0] == 0x00) /* No following entry in use. */
            break;
        if (dir[j].name[0] == 0xE5) /* Deleted file. */
            continue;
        if (dir[j].attribs == 0x0F) /* Long file name not supported */
            continue;
        if (FAT_IS_VOLUME_ID(dir[j])) /* Volume ID is not a file */
            continue;
        int i;
        for (i = 0; i < 8; i++)
            if (dir[j].name[i] != ' ') monitor_put(dir[j].name[i]);
        if (!FAT_IS_DIR(dir[j])) {
            monitor_put('.');
            for (i = 8; i < 11; ++i)
                if (dir[j].name[i] != ' ') monitor_put(dir[j].name[i]);
        }
        monitor_put('\t');
    }
    monitor_put('\n');

    /* dir[2] is SOMEFILE.TXT */
    /* Cluster number stored in table entry */
    u32int sclust = dir[2].start_cluster_lo | dir[2].start_cluster_hi << 16;

    /* Sector offset relative to start of partition */
    u32int sector = first_data_sector + root_dir_sectors +
        (sclust - 2) * fat_boot.sectors_per_cluster;

    /* Directory table no longer needed */
    kfree(dir);

    /* Load the cluster from disk */
    /* TODO should check if buffer is big enough */
    ide_ata_access(ATA_READ, drive, sector + partition->relsec,
            fat_boot.sectors_per_cluster, (u16int *) buffer);
    char *data = kmalloc(dir[2].size + 1);
    memcpy(data, buffer, dir[2].size);
    data[dir[2].size] = 0;
    monitor_print("-%s-\n", data);
    kfree(data);
}
