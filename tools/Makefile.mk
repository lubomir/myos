CLEANTARGETS += tools/gen-initrd \
		tools/gen-keymap

tools/gen-initrd : tools/gen-initrd.c
	$(call cmd,$(CC) -Wall -o $@ $^,CC,$@)

tools/gen-keymap : tools/gen-keymap.c
	$(call cmd,$(CC) -Wall -o $@ $^,CC,$@)
