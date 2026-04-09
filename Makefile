CC=gcc
CFLAGS=`sdl2-config --cflags`
LDFLAGS=`sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

all:
	$(CC) main.c background.c -o background $(CFLAGS) $(LDFLAGS)
