#include "background.h"

void initBack(Background *b, SDL_Renderer *renderer) {
    SDL_Surface *surf = IMG_Load("background.png");
    b->img = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    b->posAbsolue = (SDL_Rect){0, 0, 1280, 720}; 
    b->posEcran = (SDL_Rect){0, 0, 1280, 720};
}

void scrolling(Background *b, int direction) {
    // 0:Droite, 1:Gauche, 2:Haut, 3:Bas
    int pas = 20; 
    if (direction == 0) b->posAbsolue.x += pas;
    if (direction == 1) b->posAbsolue.x -= pas;
    if (direction == 2) b->posAbsolue.y -= pas;
    if (direction == 3) b->posAbsolue.y += pas;
}

void afficherBack(Background b, SDL_Renderer *renderer, int mode) {
    if (mode == 0) {
        SDL_RenderCopy(renderer, b.img, &b.posAbsolue, &b.posEcran);
    } else {
        // Partage d'écran (Split-screen)
        SDL_Rect v1 = {0, 0, 640, 720};
        SDL_RenderSetViewport(renderer, &v1);
        SDL_RenderCopy(renderer, b.img, &b.posAbsolue, NULL);

        SDL_Rect v2 = {640, 0, 640, 720};
        SDL_RenderSetViewport(renderer, &v2);
        SDL_RenderCopy(renderer, b.img, &b.posAbsolue, NULL);
        
        SDL_RenderSetViewport(renderer, NULL);
    }
}

void afficherTemps(SDL_Renderer *renderer, TTF_Font *font, int tempsInitial) {
    int s = (SDL_GetTicks() - tempsInitial) / 1000;
    char txt[20];
    sprintf(txt, "Temps: %d s", s);

    SDL_Color noir = {0, 0, 0};
    SDL_Surface *surf = TTF_RenderText_Blended(font, txt, noir);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect r = {1100, 20, surf->w, surf->h};

    SDL_RenderCopy(renderer, tex, NULL, &r);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void libererBack(Background *b) {
    SDL_DestroyTexture(b->img);
}
