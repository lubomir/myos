# Source files of the kernel
CSOURCES=src/common.c \
	 src/main.c \
	 src/monitor.c
ASMSOURCES=src/boot.s

# Resulting kernel image
KERNEL=src/kernel

# Linker script
LINK=src/link.ld

OBJS=$(CSOURCES:.c=.o) $(ASMSOURCES:.s=.o)
DEPS=$(CSOURCES:.c=.d)

# Tools
CC=gcc
LD=ld
ASM=nasm

CFLAGS=-nostdlib -nostdinc -fno-builtin -m32 -fno-stack-protector
LDFLAGS=-T$(LINK) -m elf_i386
ASFLAGS=-felf

all: $(KERNEL)

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

.s.o:
	$(ASM) $(ASFLAGS) $<

%.d : %.c
	@$(CC) $(CFLAGS) -MT $(@:.d=.o) -MM -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY : clean run-bochs

clean:
	-rm -f src/*.o src/*.d $(KERNEL)

floppy.img : $(KERNEL)
	sh update_image.sh

run-bochs: floppy.img
	sh run_bochs.sh
