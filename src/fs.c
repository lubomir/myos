/*
 * fs.c -- defines functions to access a file system
 * Written for JamesM's kernel development tutorial.
 */

#include "fs.h"

/*
 * The root of the filesystem.
 */
fs_node_t *fs_root = 0;

u32int read_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer)
{
    if (node->read)
        return node->read(node, offset, size, buffer);
    else
        return 0;
}

u32int write_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer)
{
    if (node->write)
        return node->write(node, offset, size, buffer);
    else
        return 0;
}

void open_fs(fs_node_t *node, u8int read, u8int write)
{
    if (node->open)
        node->open(node);
}

void close_fs(fs_node_t *node)
{
    if (node->close)
        node->close(node);
}

struct dirent * readdir_fs(fs_node_t *node, u32int index)
{
    if ((node->flags & 0x7) == FS_DIRECTORY && node->readdir)
        return node->readdir(node, index);
    else
        return 0;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name)
{
    if ((node->flags & 0x7) == FS_DIRECTORY && node->finddir)
        return node->finddir(node, name);
    else
        return 0;
}
