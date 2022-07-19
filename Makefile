stoneFile = stone.cpp
netFile = net.cpp
libFile = lib.cpp

lib: $(libFile)
	g++ -c $(libFile)

stone: $(stoneFile)
	g++ -c $(stoneFile)

net: $(netFile)
	g++ -c $(netFile)

stone: stone.o lib.o
	g++ lib.o stone.o -lsfml-graphics -lsfml-window -lsfml-system -o stone

net: net.o lib.o
	g++ lib.o net.o -lsfml-graphics -lsfml-window -lsfml-system -o net

build:
	make stone
	make net

clear:
	rm stone net stone.o net.o lib.o