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

void afficherGagnant(SDL_Renderer *renderer, TTF_Font *police, int gagnant){
    SDL_Event ev;
    int attente = 1;
    Uint32 debut = SDL_GetTicks();
    char msg[40];
    SDL_Color blanc = {255,255,255,255};
    SDL_Color jaune = {255,215,0,255};
    SDL_Surface *surf;
    SDL_Texture *tex;
    SDL_Rect dst;
    int alpha;
    Uint32 now;

    sprintf(msg, "Joueur %d Gagne !", gagnant);

    while(attente){
        while(SDL_PollEvent(&ev)){
            if(ev.type==SDL_QUIT) attente=0;
            if(ev.type==SDL_KEYDOWN) attente=0;
        }
        now = SDL_GetTicks() - debut;
        alpha = (int)(128 + 127 * SDL_sinf((float)now / 400.0f));
        if(alpha<0) alpha=0; if(alpha>255) alpha=255;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_Rect fond = {SCREEN_W/2-250, SCREEN_H/2-100, 500, 200};
        SDL_RenderFillRect(renderer, &fond);
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderDrawRect(renderer, &fond);

        surf = TTF_RenderText_Blended(police, msg, jaune);
        if(surf){
            tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            SDL_SetTextureAlphaMod(tex, (Uint8)alpha);
            SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
            dst.x = SCREEN_W/2 - dst.w/2;
            dst.y = SCREEN_H/2 - dst.h/2 - 20;
            SDL_RenderCopy(renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }

        surf = TTF_RenderText_Blended(police, "Appuyez sur une touche pour quitter", blanc);
        if(surf){
            tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
            dst.x = SCREEN_W/2 - dst.w/2;
            dst.y = SCREEN_H/2 + 40;
            SDL_RenderCopy(renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

int main()
{
    SDL_Window *window;
    SDL_Texture *bgTex, *obsTex;
    SDL_Surface *bgSurf, *obsSurf;
    SDL_Renderer *renderer;
    perso p1, p2;
    Obstacle obstacle;
    int running = 1;
    SDL_Event event;
    Uint32 start, dt, lastTime, c1_cd = 0, c2_cd = 0;
    int i;

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)!=0){ printf("SDL_Init Error: %s\n", SDL_GetError()); return 1; }
    if(IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG)==0){ printf("IMG_Init Error: %s\n", IMG_GetError()); return 1; }
    if(TTF_Init()==-1){ printf("TTF_Init Error: %s\n", TTF_GetError()); return 1; }

    window = SDL_CreateWindow("SDL2 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, 0);
    // Force SOFTWARE pour compatibilité VM
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    bgSurf = IMG_Load("bg.jpg");
    if(!bgSurf){ printf("Erreur chargement bg.jpg\n"); return 1; }
    bgTex = SDL_CreateTextureFromSurface(renderer, bgSurf);
    SDL_FreeSurface(bgSurf);

    obsSurf = IMG_Load("obstacle-removebg-preview.png");
    if(obsSurf){
        obsTex = SDL_CreateTextureFromSurface(renderer, obsSurf);
        SDL_FreeSurface(obsSurf);
    } else {
        printf("Erreur chargement obstacle image\n");
        obsTex = NULL;
    }

    initPerso(&p1, renderer);
    initialiserJoueur2(&p2, renderer);

    obstacle.pos.w = 60; 
    obstacle.pos.h = 60;
    obstacle.pos.x = 400; 
    obstacle.pos.y = SCREEN_H - obstacle.pos.h - 20; 
    obstacle.active = 1;

    lastTime = SDL_GetTicks();

    while(running)
    {
        start = SDL_GetTicks();
        dt = start - lastTime;
        lastTime = start;

        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT) running = 0;
            if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_RIGHT) p1.direction = 1;
                else if(event.key.keysym.sym == SDLK_LEFT) p1.direction = 0;
                else if(event.key.keysym.sym == SDLK_SPACE && p1.up==0){ p1.jump_V=-15; p1.up=1; }
                else if(event.key.keysym.sym == SDLK_LSHIFT) p1.isRunning = 1;
                else if(event.key.keysym.sym == SDLK_LCTRL) attaquerPerso(&p1);

                if(event.key.keysym.sym == SDLK_d) p2.direction = 1;
                else if(event.key.keysym.sym == SDLK_a) p2.direction = 0;
                else if(event.key.keysym.sym == SDLK_w && p2.up==0){ p2.jump_V=-15; p2.up=1; }
                else if(event.key.keysym.sym == SDLK_r) p2.isRunning = 1;
                else if(event.key.keysym.sym == SDLK_t) attaquerPerso(&p2);
            }
            if(event.type == SDL_KEYUP)
            {
                if(event.key.keysym.sym == SDLK_RIGHT && p1.direction == 1) p1.direction=-1;
                if(event.key.keysym.sym == SDLK_LEFT && p1.direction == 0) p1.direction=-1;
                if(event.key.keysym.sym == SDLK_LSHIFT) p1.isRunning = 0;
                
                if(event.key.keysym.sym == SDLK_d && p2.direction == 1) p2.direction=-1;
                if(event.key.keysym.sym == SDLK_a && p2.direction == 0) p2.direction=-1;
                if(event.key.keysym.sym == SDLK_r) p2.isRunning = 0;
            }
        }

        deplacerPerso(&p1, dt); animerPerso(&p1);
        deplacerPerso(&p2, dt); animerPerso(&p2);

        if(obstacle.active && checkCollision(p1.posPerso, obstacle.pos)) {
            if(SDL_GetTicks() > c1_cd) { p1.vie--; c1_cd = SDL_GetTicks() + 1000; }
        }
        if(obstacle.active && checkCollision(p2.posPerso, obstacle.pos)) {
            if(SDL_GetTicks() > c2_cd) { p2.vie--; c2_cd = SDL_GetTicks() + 1000; }
        }

        if(p1.direction != -1) p1.score++;
        if(p2.direction != -1) p2.score++;

        if(p1.vie < 0 || p2.vie < 0) running = 0;


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTex, NULL, NULL);

        if(obstacle.active){ 
            if(obsTex) SDL_RenderCopy(renderer, obsTex, NULL, &obstacle.pos);
            else { SDL_SetRenderDrawColor(renderer, 255,0,0,255); SDL_RenderFillRect(renderer, &obstacle.pos); }
        }

        afficherPerso(&p1, renderer);
        afficherPerso(&p2, renderer);

        SDL_RenderPresent(renderer);

        // Délai manuel stable (60 FPS) pour éviter le clignotement VMware
        if(16 > (SDL_GetTicks() - start)) SDL_Delay(16 - (SDL_GetTicks() - start));
    }

    {
        int gagnant = 0;
        TTF_Font *fontFin = TTF_OpenFont("font.ttf", 48);
        if(!fontFin) fontFin = p1.police;
        if(p1.vie < 0) gagnant = 2;
        else if(p2.vie < 0) gagnant = 1;
        if(gagnant > 0 && fontFin)
            afficherGagnant(renderer, fontFin, gagnant);
        if(fontFin && fontFin != p1.police) TTF_CloseFont(fontFin);
    }

    if(p1.spriteTex) SDL_DestroyTexture(p1.spriteTex);
    if(p1.heartTex) SDL_DestroyTexture(p1.heartTex);
    for(i=0; i<MAX_LIVES; i++) if(p1.viesTex[i]) SDL_DestroyTexture(p1.viesTex[i]);
    if(p1.police) TTF_CloseFont(p1.police);
    if(p2.spriteTex) SDL_DestroyTexture(p2.spriteTex);
    if(p2.heartTex) SDL_DestroyTexture(p2.heartTex);
    for(i=0; i<MAX_LIVES; i++) if(p2.viesTex[i]) SDL_DestroyTexture(p2.viesTex[i]);
    if(p2.police) TTF_CloseFont(p2.police);
    if(obsTex) SDL_DestroyTexture(obsTex);
    SDL_DestroyTexture(bgTex); SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window);
    TTF_Quit(); IMG_Quit(); SDL_Quit();
    return 0;
}
