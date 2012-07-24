/*
 * syscall.c -- defines the implementation of a system call system.
 * Written for JamesM's kernel development tutorial.
 */

#include "isr.h"
#include "syscall.h"

#include "monitor.h"

static void syscall_handler(registers_t regs);

static void *syscalls[2] = {
    &monitor_write,
};
u32int num_syscalls = 1;

void initialise_syscalls(void)
{
    /* Register our syscall handler. */
    register_interrupt_handler(0x80, &syscall_handler);
}

void syscall_handler(registers_t regs)
{
    /* First check if the requested syscall number is valid.
     * The syscall number is found in EAX. */
    if (regs.eax >= num_syscalls)
        PANIC("Bad syscall");   /* This should rather be simple return. */

    /* Get the required syscall location. */
    void *location = syscalls[regs.eax];

    /* We don't know how many parameters the function wants, so we just push
     * them all onto the stack in the correct order. The function will use
     * all the parameters it wants and we can pop them all back afterwards. */
    int ret;
    asm volatile (
            "push %1;"
            "push %2;"
            "push %3;"
            "push %4;"
            "push %5;"
            "call *%6;"
            "pop %%ebx;"
            "pop %%ebx;"
            "pop %%ebx;"
            "pop %%ebx;"
            "pop %%ebx;"
            : "=a" (ret)
            : "r" (regs.edi), "r" (regs.esi), "r" (regs.edx),
              "r" (regs.ecx), "r" (regs.ebx), "r" (location)
            );
    regs.eax = ret;
}

DEFN_SYSCALL1(monitor_write, 0, const char *)
