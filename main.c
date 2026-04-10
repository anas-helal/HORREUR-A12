
#include "person.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { SDL_Rect pos; int active; } Obstacle;

int checkCollision(SDL_Rect a, SDL_Rect b){
    if(a.y+a.h <= b.y) return 0;
    if(a.y >= b.y+b.h) return 0;
    if(a.x+a.w <= b.x) return 0;
    if(a.x >= b.x+b.w) return 0;
    return 1;
}

int main()
{
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)!=0){ printf("SDL Error: %s\n",SDL_GetError()); return 1; }
    if(IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG)==0){ printf("IMG Error: %s\n",IMG_GetError()); return 1; }
    if(TTF_Init()==-1){ printf("TTF Error: %s\n",TTF_GetError()); return 1; }

    SDL_Window *window = SDL_CreateWindow("SDL2 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Surface *bgSurf = IMG_Load("bg.jpg");
    if(!bgSurf){ printf("Erreur bg.jpg: %s\n", IMG_GetError()); return 1; }
    SDL_Texture *bgTex = SDL_CreateTextureFromSurface(renderer, bgSurf);
    SDL_FreeSurface(bgSurf);

    // Initialisation joueur
    perso p1;
    initPerso(&p1, renderer);

    // Obstacle
    Obstacle obstacle = {{400, SCREEN_H - 40, 30, 40}, 1};

    int FPS = 60;
    int running = 1;
    SDL_Event event;
    Uint32 start, dt;

    while(running)
    {
        start = SDL_GetTicks();

        // Event
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT) running = 0;
            if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_RIGHT) p1.direction = 1;
                if(event.key.keysym.sym == SDLK_LEFT) p1.direction = 0;
                if(event.key.keysym.sym == SDLK_SPACE && p1.up==0){ p1.jump_V=-p1.jump_height; p1.up=1; }
            }
            if(event.type == SDL_KEYUP)
            {
                if(event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT) p1.direction=-1;
            }
        }

        dt = SDL_GetTicks() - start;
        deplacerPerso(&p1, dt);
        animerPerso(&p1);

        if(obstacle.active && checkCollision(p1.posPerso, obstacle.pos)) decreaseLife(&p1);

        // Rendu
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTex, NULL, NULL);

        // obstacle
        if(obstacle.active){
            SDL_SetRenderDrawColor(renderer, 255,0,0,255);
            SDL_RenderFillRect(renderer, &obstacle.pos);
        }

        afficherPerso(&p1, renderer);

        SDL_RenderPresent(renderer);

        // Limiter FPS
        if(1000/FPS > SDL_GetTicks()-start)
            SDL_Delay(1000/FPS - (SDL_GetTicks()-start));
    }

    freePerso(&p1);
    SDL_DestroyTexture(bgTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
