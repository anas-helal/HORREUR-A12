all: run

prog: main.o enemy.o
	gcc main.o enemy.o `sdl2-config --libs` -lSDL2_image -o prog -g

main.o: main.c enemy.h
	gcc -c main.c -g `sdl2-config --cflags`

enemy.o: enemy.c enemy.h
	gcc -c enemy.c -g `sdl2-config --cflags`

clean:
	rm -f *.o prog

run: prog
	./prog

.PHONY: all run clean
