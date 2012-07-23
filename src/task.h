/*
 * task.h -- Defines the structures and prototypes needed to multitask.
 * Written for JamesM's kernel development tutorial.
 */

#ifndef TASK_H
#define TASK_H

#include "common.h"

/*
 * Causes the current process's stack to be forcibly move to a new location.
 */
void move_stack(void *new_stack_start, u32int size);

#endif /* end of include guard: TASK_H */
