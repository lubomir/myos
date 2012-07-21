CLEANTARGETS += tools/gen-initrd

tools/gen-initrd : tools/gen-initrd.c
	@echo " CC   $@"
	@$(CC) -Wall -o $@ $^
