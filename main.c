#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include "minimap.h"

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    SDL_Window* window = SDL_CreateWindow("Shadow of the Clown - Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1080, 720, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface* surf_bg = IMG_Load("background_jeuvideo.png");
    SDL_Texture* background_texture = SDL_CreateTextureFromSurface(renderer, surf_bg);
    SDL_FreeSurface(surf_bg);
    Minimap m;
    m.player_texture = IMG_LoadTexture(renderer, "ball.png");
    m.player_pos.w = 100;
    m.player_pos.h = 100;
    init_minimap(&m, renderer);
    SDL_Rect joueur_reel = {100, 100, 50, 50};
    SDL_Rect camera = {0,0,1080, 720};
    int redimensionnement = 10;
    int running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
        }
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_RIGHT]) joueur_reel.x += 5;
        if (state[SDL_SCANCODE_LEFT])  joueur_reel.x -= 5;
        if (state[SDL_SCANCODE_DOWN])  joueur_reel.y += 5;
        if (state[SDL_SCANCODE_UP])    joueur_reel.y -= 5;
        MAJminimap(joueur_reel, &m, camera, redimensionnement);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background_texture, NULL, NULL);
        SDL_RenderCopy(renderer, m.player_texture, NULL, &joueur_reel);
        afficher(m, renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyTexture(background_texture);
    free_minimap(&m);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
