
all:
	@gcc ctags_snippet.c -c -fPIC $(shell pkg-config --cflags geany) #-Wall

	@gcc ctags_snippet.o --share $(shell pkg-config --libs geany) -o ctags_snippet.so #-Wall

	@mkdir -p ~/.local/usr/lib/geany/plugin/

	@cp --backup=none ctags_snippet.so ~/.local/usr/lib/geany/plugin/

