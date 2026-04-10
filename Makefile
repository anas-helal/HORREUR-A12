prog: main.o source.o
	gcc main.o source.o -o prog -g -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
main.o: main.c
	gcc -c main.c -g
source.o: source.c
	gcc -c source.c -g
