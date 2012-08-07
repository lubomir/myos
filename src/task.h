/**
 * @file task.h
 *
 * Defines the structures and prototypes needed to multitask.
 *
 * Written for JamesM's kernel development tutorial.
 */

#ifndef TASK_H
#define TASK_H

#include "common.h"
#include "paging.h"

/** Use a 2kB kernel stack. */
#define KERNEL_STACK_SIZE 2048

/** This structure defines a 'task' – a process. */
typedef struct task {
    /** Process ID. */
    int id;
    /** Stack and base pointers. */
    u32int esp, ebp;
    /** Instruction pointer. */
    u32int eip;
    /** Page directory. */
    page_directory_t *page_directory;
    /** Kernel stack location. */
    u32int kernel_stack;
    /** The next task in a linked list. */
    struct task *next;
} task_t;

/**
 * Initialises the tasking system.
 */
void initialise_tasking(void);

/**
 * Call by the timer hook, this changes the running process.
 */
void switch_task(void);

/**
 * Forks the current process, spawning a new one with a different
 * memory space.
 */
int fork(void);

/**
 * Causes the current process's stack to be forcibly move to a new location.
 *
 * @param new_stack_start   new start of stack
 * @param size              size of the stack
 */
void move_stack(void *new_stack_start, u32int size);

/**
 * Returns the pid of current process.
 */
int getpid(void);

/**
 * Switch the execution to user mode.
 */
void switch_to_user_mode(void);

#endif /* end of include guard: TASK_H */
