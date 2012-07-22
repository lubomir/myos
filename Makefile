# Source files of the kernel
CSOURCES=src/common.c \
	 src/descriptor-tables.c \
	 src/fs.c \
	 src/initrd.c \
	 src/isr.c \
	 src/kheap.c \
	 src/kmain.c \
	 src/monitor.c \
	 src/ordered-array.c \
	 src/paging.c \
	 src/timer.c
ASMSOURCES=src/boot.s \
	   src/gdt.s \
	   src/interrupt.s

# Resulting kernel image
KERNEL=src/kernel
INITRD=initrd.img

# Linker script
LINK=src/link.ld

OBJS=$(CSOURCES:.c=.o) $(ASMSOURCES:.s=.o)
DEPS=$(CSOURCES:.c=.dep)

# System tools
CC=gcc
LD=ld
ASM=nasm

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector \
       -m32 -Wall -Iinclude -ggdb3
LDFLAGS=-T$(LINK) -m elf_i386
ASFLAGS=-felf

all: $(KERNEL) $(TOOLS)

$(KERNEL): $(OBJS) $(LINK)
	@echo " LD   $@"
	@$(LD) $(LDFLAGS) -o $@ $^

$(INITRD): tools/gen-initrd
	sh tools/update_initrd.sh

.s.o:
	@echo " ASM  $@"
	@$(ASM) $(ASFLAGS) $<

%.o : %.c
	@echo " CC   $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

%.dep : %.c
	@$(CC) $(CFLAGS) -MT "$(<:.c=.o) $@" -MM -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

include tools/Makefile.mk

.PHONY : clean cloc run-bochs debug-bochs run-qemu debug-qemu

clean:
	-rm -f src/*.o src/*.d $(KERNEL)
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
