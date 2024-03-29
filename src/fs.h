/**
 * @file    fs.h
 *
 * Defines interface and structures for accessing a file system.
 *
 * Written for JamesM's kernel development tutorial.
 */

#ifndef FS_H
#define FS_H

#include "common.h"

#define FS_FILE         0x01    /**< Whether node is a file */
#define FS_DIRECTORY    0x02    /**< Whether node is a directory */
#define FS_CHARDEVICE   0x03    /**< Whether node is a character device */
#define FS_BLOCKDEVICE  0x04    /**< Whether node is a block device */
#define FS_PIPE         0x05    /**< Whether node is a pipe */
#define FS_SYMLINK      0x06    /**< Whether node is a symbolic link */
#define FS_MOUNTPOINT   0x08    /**< Whether node is an active mountpoint? */

struct fs_node;

typedef u32int (*read_type_t)   (struct fs_node*, u32int, u32int, u8int*);
typedef u32int (*write_type_t)  (struct fs_node*, u32int, u32int, u8int*);
typedef void   (*open_type_t)   (struct fs_node*);
typedef void   (*close_type_t)  (struct fs_node*);
typedef struct dirent * (*readdir_type_t) (struct fs_node*, u32int);
typedef struct fs_node * (*finddir_type_t) (struct fs_node*, char *name);

/** Node in a file system tree. */
typedef struct fs_node {
    /** The filename. */
    char name[128];
    /** The permissions mask. */
    u32int mask;
    /** The owning user. */
    u32int uid;
    /** The owning group. */
    u32int gid;
    /** Includes the node type, see macros above. */
    u32int flags;
    /** This is device specific - filesystem identification. */
    u32int inode;
    /** Size of the file, in bytes. */
    u32int length;
    /** An implementation defined number. */
    u32int impl;
    
    read_type_t     read;
    write_type_t    write;
    open_type_t     open;
    close_type_t    close;
    readdir_type_t  readdir;
    finddir_type_t  finddir;

    /** Used by mountpoints and symlinks. */
    struct fs_node *ptr;
} fs_node_t;

/** Directory entry. */
struct dirent {
    /** Filename. */
    char name[128];
    /** I-node number. Required by POSIX. */
    u32int ino;
};

/**
 * The root node of the filesystem.
 * Defined in fs.c.
 */
extern fs_node_t *fs_root;

/*
 * Standard read/write/open/close functions. Note that these are all suffixed
 * with _fs to distinguish them from the read/write/open/close which deal with
 * file descriptors, not file nodes.
 */
u32int read_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer);
u32int write_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer);
void open_fs(fs_node_t *node, u8int read, u8int write);
void close_fs(fs_node_t *node);
struct dirent * readdir_fs(fs_node_t *node, u32int index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);

#endif /* end of include guard: FS_H */
