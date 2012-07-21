# Source files of the kernel
CSOURCES=src/common.c \
	 src/descriptor-tables.c \
	 src/fs.c \
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

# Linker script
LINK=src/link.ld

OBJS=$(CSOURCES:.c=.o) $(ASMSOURCES:.s=.o)
DEPS=$(CSOURCES:.c=.dep)

# Tools
CC=gcc
LD=ld
ASM=nasm

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector \
       -m32 -Wall -Iinclude -ggdb3
LDFLAGS=-T$(LINK) -m elf_i386
ASFLAGS=-felf

all: $(KERNEL)

$(KERNEL): $(OBJS) $(LINK)
	@echo " LD   $@"
	@$(LD) $(LDFLAGS) -o $@ $^

.s.o:
	@echo " ASM  $@"
	@$(ASM) $(ASFLAGS) $<

%.o : %.c
	@echo " CC   $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

%.dep : %.c
	@$(CC) $(CFLAGS) -MT $(<:.c=.o) -MM -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY : clean run-bochs debug-bochs run-qemu debug-qemu

clean:
	-rm -f src/*.o src/*.d $(KERNEL)

floppy.img : $(KERNEL)
	sh update_image.sh

run-bochs: floppy.img
	@bochs -q

debug-bochs: floppy.img
	@bochs -q 'gdbstub: enabled=1'

run-qemu : $(KERNEL)
	@qemu -kernel $(KERNEL)

debug-qemu : $(KERNEL)
	@qemu -kernel $(KERNEL) -s -S
