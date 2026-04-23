prog:main.o person.o
	gcc -o prog main.o person.o -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm
main.o:main.c
	gcc -c main.c -g
person.o:person.c
	gcc -c person.c -g
clean:
	rm -f *.o prog
