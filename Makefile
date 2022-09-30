mainFile = net.cpp
libFile = lib.cpp
comp = "g++-8"

lib: $(libFile)
	$(comp) -c $(libFile)

net: $(mainFile)
	$(comp) -c $(mainFile)

build: lib.o net.o
	$(comp) lib.o net.o -lsfml-graphics -lsfml-window -lsfml-system -Ofast -o net

clear:
	rm net net.o lib.o
