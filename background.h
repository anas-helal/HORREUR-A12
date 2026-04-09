#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    SDL_Texture *img;        
    SDL_Rect posAbsolue;     // Pour le scrolling 4 sens
    SDL_Rect posEcran;       
} Background;

// Fonctions demandées par ton lot
void initBack(Background *b, SDL_Renderer *renderer);
void afficherBack(Background b, SDL_Renderer *renderer, int mode); // mode 0: mono, 1: multi
void scrolling(Background *b, int direction); 
void afficherTemps(SDL_Renderer *renderer, TTF_Font *font, int tempsInitial);
void libererBack(Background *b);

#endif
