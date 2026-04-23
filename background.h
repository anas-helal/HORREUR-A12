#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    SDL_Texture *img1;
    SDL_Texture *img2;

    SDL_Rect camera1;
    SDL_Rect camera2;

    SDL_Rect posEcran1;
    SDL_Rect posEcran2;

    int width1, height1;
    int width2, height2;

} Background;

void initBackground(Background *b, SDL_Renderer *renderer, char *path);
void loadBackground1(Background *b, SDL_Renderer *renderer, char *path);
void loadBackground2(Background *b, SDL_Renderer *renderer, char *path);

void afficherBackground(Background b, SDL_Renderer *renderer, int mode);
void scrolling(Background *b, int dx, int dy, int numBack);

void afficherInterface(SDL_Renderer *renderer, int score, int vie, int temps, TTF_Font *font);

#endif
