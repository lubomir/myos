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

#endif /* end of include guard: IDE_H */
