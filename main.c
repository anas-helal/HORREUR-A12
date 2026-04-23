#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "background.h"

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280, 720, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    Background b;
    initBackground(&b, renderer, "background1.png");

    TTF_Font *font = TTF_OpenFont("arial.ttf", 24);

    int running = 1;
    SDL_Event e;

    int score = 0, vie = 3;
    Uint32 start = SDL_GetTicks();

    int level1_done = 0;
    int level2_done = 0;

    while(running)
    {
        while(SDL_PollEvent(&e))
            if(e.type == SDL_QUIT) running = 0;

        const Uint8 *k = SDL_GetKeyboardState(NULL);

        
        if(k[SDL_SCANCODE_RIGHT]) scrolling(&b,5,0,1);
        if(k[SDL_SCANCODE_LEFT])  scrolling(&b,-5,0,1);

        
        if(k[SDL_SCANCODE_D]) scrolling(&b,5,0,2);
        if(k[SDL_SCANCODE_Q]) scrolling(&b,-5,0,2);

        
        if(!level1_done && b.camera1.x >= b.width1 - b.camera1.w)
        {
            level1_done = 1;
            loadBackground1(&b, renderer, "background2.png");
            b.camera1.x = 0;
        }

        if(!level2_done && b.camera2.x >= b.width2 - b.camera2.w)
        {
            level2_done = 1;
            loadBackground2(&b, renderer, "background2.png");
            b.camera2.x = 0;
        }

        int temps = (SDL_GetTicks() - start)/1000;

        SDL_RenderClear(renderer);

        afficherBackground(b, renderer, 1);
        afficherInterface(renderer, score, vie, temps, font);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
