CLEANTARGETS += tools/gen-initrd

tools/gen-initrd : tools/gen-initrd.c
	$(call cmd,$(CC) -Wall -o $@ $^,CC,$@)
