CLEANTARGETS += gen-initrd

gen-initrd : tools/gen-initrd.c
	@echo " CC   $@"
	@$(CC) -Wall -o $@ $^

