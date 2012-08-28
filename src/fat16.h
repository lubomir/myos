/*
 * fat16.h -- FAT16 file-system driver
 */

#ifndef FAT16_H
#define FAT16_H

#include "common.h"
#include "partition-table.h"

typedef struct {
    u8int   bios_drive_num;
    u8int   reserved1;
    u8int   boot_signature;
    u32int  volume_id;
    u8int   volume_label[11];
    u8int   fat_type_label[8];
} PACKED fat_extbs_16_t;

typedef struct {
    u8int   bootjmp[3];
    u8int   oem_name[8];
    u16int  bytes_per_sector;
    u8int   sectors_per_cluster;
    u16int  reserved_sector_count;
    u8int   table_count;
    u16int  root_entry_count;
    u16int  total_sectors_16;
    u8int   media_type;
    u16int  table_size16;
    u16int  sectors_per_track;
    u16int  head_side_count;
    u32int  hidden_sector_count;
    u32int  total_sectors_32;

    u8int   extended_section[54];
} PACKED fat_bs_t;

#define FAT_READ_ONLY   0x01
#define FAT_HIDDEN      0x02
#define FAT_SYSTEM      0x04
#define FAT_VOLUME_ID   0x08
#define FAT_DIRECTORY   0x10
#define FAT_ARCHIVE     0x20

typedef struct {
    u8int   name[11];
    u8int   attribs;
    u8int   reserved;
    /* tenths of a second */
    u8int   ctime_s;
    /* 5 bits hour, 6 bits minutes, 5 bits seconds */
    u16int  ctime_t;
    /* 7 bits year, 4 bits month, 5 bits day */
    u16int  ctime_d;
    u16int  atime_d;
    u16int  start_cluster_hi;
    u16int  mtime_t;
    u16int  mtime_d;
    u16int  start_cluster_lo;
    /* Size in bytes. */
    u32int  size;
} fat_dir_t;

/*
 */
void fat_setup(u8int drive, pt_entry_t *partition);

#endif /* end of include guard: FAT16_H */
