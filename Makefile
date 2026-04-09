CC=gcc
SRC=src/main.c src/core.c src/textures.c src/audio.c src/scores.c src/ui.c src/game.c src/tictactoe.c src/perso.c
OUT=submenu

CFLAGS=`sdl2-config --cflags` -Iinclude
LIBS=`sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LIBS)

clean:
	rm -f $(OUT)
