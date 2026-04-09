#include "background.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window *win = SDL_CreateWindow("Projet Batman", 100, 100, 1280, 720, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, 0);

    Background b;
    initBack(&b, renderer);
    TTF_Font *f = TTF_OpenFont("font.ttf", 24);
    int tInit = SDL_GetTicks();
    int running = 1, mode = 0;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RIGHT) scrolling(&b, 0);
                if (e.key.keysym.sym == SDLK_LEFT)  scrolling(&b, 1);
                if (e.key.keysym.sym == SDLK_UP)    scrolling(&b, 2);
                if (e.key.keysym.sym == SDLK_DOWN)  scrolling(&b, 3);
                if (e.key.keysym.sym == SDLK_m)     mode = !mode; // M pour changer de mode
            }
        }
        SDL_RenderClear(renderer);
        afficherBack(b, renderer, mode);
        afficherTemps(renderer, f, tInit);
        SDL_RenderPresent(renderer);
    }

    libererBack(&b);
    return 0;
}
