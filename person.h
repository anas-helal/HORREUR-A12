#ifndef DER_H_
#define DER_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_H 400
#define SCREEN_W 900
#define MAX_LIVES 5

typedef struct
{
    SDL_Texture *spriteTex;
    SDL_Texture *viesTex[MAX_LIVES];

    SDL_Rect posPerso;
    SDL_Rect posSprite;
    SDL_Rect posVie;
    SDL_Rect posScore;

    TTF_Font *police;

    int direction;
    int frame;
    int score;

    int vie;
    int maxVie;

    Uint32 lastScoreTime;
    int moveDistance;

    int jump_height;
    int jump_V;
    int up;

    int speed;
    float acceleration;

    int isActive;

} perso;

// Fonctions SDL2
void initPerso(perso *p, SDL_Renderer *renderer);
void afficherPerso(perso *p, SDL_Renderer *renderer);
void deplacerPerso(perso *p, Uint32 dt);
void animerPerso(perso *p);
void freePerso(perso *p);
void ajouterPerso(perso *p, SDL_Renderer *renderer);
void updateScore(perso *p, Uint32 currentTime);
void updateLifeBar(perso *p);
void decreaseLife(perso *p);

#endif
