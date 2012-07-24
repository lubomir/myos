# Source files of the kernel
SOURCES=src/boot.s \
	src/common.c \
	src/descriptor-tables.c \
	src/fs.c \
	src/gdt.s \
	src/initrd.c \
	src/interrupt.s \
	src/isr.c \
	src/kheap.c \
	src/kmain.c \
	src/monitor.c \
	src/ordered-array.c \
	src/paging.c \
	src/process.s \
	src/task.c \
	src/timer.c \
	src/syscall.c

# Resulting kernel image
KERNEL=src/kernel
INITRD=initrd.img

# Linker script
LINK=src/link.ld

OBJS := $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(SOURCES)))
DEPS := $(patsubst %.c,%.dep,$(filter %.c,$(SOURCES)))

# System tools
CC=gcc
LD=ld
ASM=nasm

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector \
       -m32 -Iinclude -ggdb3 \
       -Wall -Wextra -Wpointer-arith -Wcast-align -Wredundant-decls \
       -Wnested-externs
LDFLAGS=-T$(LINK) -m elf_i386
ASFLAGS=-felf

ifeq ($(V),1)
cmd = $1
else
cmd = @printf " %s\t%s\n" $2 $3; $1
endif

all: $(KERNEL) $(TOOLS)

$(KERNEL): $(OBJS) $(LINK)
	$(call cmd,$(LD) $(LDFLAGS) -o $@ $^,LD,$@)

$(INITRD): tools/gen-initrd
	sh tools/update_initrd.sh

%.o : %.s
	$(call cmd,$(ASM) $(ASFLAGS) $<,ASM,$@)

%.o : %.c
	$(call cmd,$(CC) $(CFLAGS) -c -o $@ $<,CC,$@)

%.dep : %.c
	@$(CC) $(CFLAGS) -MT "$(<:.c=.o) $@" -MM -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

include tools/Makefile.mk

.PHONY : clean cloc run-bochs debug-bochs run-qemu debug-qemu

clean:
	-rm -f src/*.o src/*.dep $(KERNEL)
	-rm -f $(CLEANTARGETS)

cloc:
	@cloc . --exclude-dir=tmp --force-lang=make,mk

floppy.img : $(KERNEL)
	sh tools/update_image.sh

run-bochs: floppy.img
	@bochs -q

debug-bochs: floppy.img
	@bochs -q 'gdbstub: enabled=1'

run-qemu : $(KERNEL) $(INITRD)
	@qemu -kernel $(KERNEL) -initrd $(INITRD)

debug-qemu : $(KERNEL) $(INITRD)
	@qemu -kernel $(KERNEL) -initrd $(INITRD) -s -S
