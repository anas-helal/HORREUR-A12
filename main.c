#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include "minimap.h"

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    SDL_Window* window = SDL_CreateWindow("Shadow of the Clown - Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1080, 720, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface* surf_bg = IMG_Load("house_haunted.jpg");
    SDL_Texture* background_texture = SDL_CreateTextureFromSurface(renderer, surf_bg);
    SDL_FreeSurface(surf_bg);
    SousMenu sm = init_SousMenu(renderer);
    Minimap m;
    m.player_texture = IMG_LoadTexture(renderer, "ball.png");
    m.player_pos.w = 100;
    m.player_pos.h = 100;
    init_minimap(&m, renderer);
    Entite feu;
    feu.texture = IMG_LoadTexture(renderer, "fire.png");
    feu.pos = (SDL_Rect){500, 300, 60, 60};
    SDL_Rect joueur = {100, 100, 50, 50};
    SDL_Rect camera = {0,0,1080, 720};
    int redimensionnement = 10;
    int score = 100;
    int running = 1;
    int interface = 3;
    int vies = 3;
    SDL_Event e;
    while (running) {
        int input=0;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
        }
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_RIGHT]) joueur.x += 3;
        if (state[SDL_SCANCODE_LEFT])  joueur.x -= 3;
        if (state[SDL_SCANCODE_DOWN])  joueur.y += 3;
        if (state[SDL_SCANCODE_UP])    joueur.y -= 3;
        if (collision(joueur, feu.pos)) {
            vies--;
            score -= 10;
            printf("Collision ! Vies restantes: %d | Score: %d\n", vies, score);
            joueur.x -= 20; 
        }
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_l) {
                interface = 4;
                if (sm.musique) Mix_PlayMusic(sm.musique, -1);
            }
            if (interface == 4) {
                if (e.key.keysym.sym == SDLK_UP) input = 1;
                if (e.key.keysym.sym == SDLK_DOWN) input = 2;
                if (e.key.keysym.sym == SDLK_RETURN) input = 3;
            }
            if (e.key.keysym.sym == SDLK_l) {
                charger_jeu(&score, &vies, &joueur, &camera);
            }
        }
        if (vies <= 0) running = 0;
        MAJminimap(joueur, &m, camera, redimensionnement);
        SDL_RenderClear(renderer);
        if (interface == 3) {
            MAJminimap(joueur, &m, camera, 10);
            afficher(m, renderer);
            SDL_RenderCopy(renderer, m.player_texture, NULL, &joueur);
        } 
        else if (interface == 4) {
            afficher(m, renderer); 
            afficher_SousMenu(sm, renderer);
            int action = update_SousMenu(&sm, input);
            if (action == -1) {
                sauvegarder_jeu(score, vies, joueur, camera);
                interface = 3;
                Mix_HaltMusic();
            } else {
                interface = action;
                if (interface == 3) Mix_HaltMusic();
            }
        }
        SDL_RenderCopy(renderer, background_texture, NULL, NULL);
        SDL_RenderCopy(renderer, feu.texture, NULL, &feu.pos);
        SDL_RenderCopy(renderer, m.player_texture, NULL, &joueur);
        afficher(m, renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyTexture(feu.texture);
    SDL_DestroyTexture(background_texture);
    free_minimap(&m);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
/**
* @file main.c
* @brief Testing Program.
* @author C Team
* @version 0.1
* @date May 03, 2026
*
* Testing program for background scrollilng
*
*/
