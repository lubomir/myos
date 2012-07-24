CLEANTARGETS += data/keymaps/us.keymap

%.keymap : %.txt tools/gen-keymap
	./tools/gen-keymap $< $@
