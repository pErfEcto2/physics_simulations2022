mainFile = net.cpp
libFile = lib.cpp

lib: $(libFile)
	g++ -c $(libFile)

net: $(mainFile)
	g++ -c $(mainFile)

build: lib.o net.o
	g++ lib.o net.o -lsfml-graphics -lsfml-window -lsfml-system -Ofast -o net

clear:
	rm net net.o lib.o