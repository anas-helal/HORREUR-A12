#ifndef MINIMAP_H
#define MINIMAP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
    SDL_Texture *map_texture;
    SDL_Texture *player_texture;
    SDL_Rect map_pos;      // Position de la minimap à l'écran
    SDL_Rect player_pos;   // Position du point rouge sur la carte
} Minimap;

void init_minimap(Minimap *m, SDL_Renderer *renderer);
void MAJminimap(SDL_Rect posJoueur,Minimap *m,SDL_Rect camera,int redimensionnement);
void afficher(Minimap m, SDL_Renderer *renderer);
void free_minimap(Minimap *m);

#endif
