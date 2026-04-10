#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include "minimap.h"

void init_minimap(Minimap *m, SDL_Renderer *renderer) {
    // 1. Chargement des textures
    m->map_texture = IMG_LoadTexture(renderer, "background_jeuvideo.png");
    m->player_texture = IMG_LoadTexture(renderer, "ball.png");

    // 2. Position de la minimap sur l'interface (ex: en haut à droite)
    m->map_pos.x = 20; 
    m->map_pos.y = 20;
    m->map_pos.w = 200; // Largeur de la minimap
    m->map_pos.h = 150; // Hauteur de la minimap

    // 3. Taille initiale du point joueur sur la minimap
    m->player_pos.w = 10;
    m->player_pos.h = 10;
}

void MAJminimap(SDL_Rect posJoueur,Minimap *m,SDL_Rect camera,int redimensionnement) {
    SDL_Rect posJoueurABS;
    posJoueurABS.x=posJoueur.x+camera.x;
    posJoueurABS.y=posJoueur.y+camera.y;
    m->player_pos.x=m->map_pos.x+(posJoueurABS.x+redimensionnement)/100;
    m->player_pos.y=m->map_pos.y+(posJoueurABS.y+redimensionnement)/100;
}

void afficher(Minimap m, SDL_Renderer *renderer) {
    // Affichage du fond de la carte
    SDL_RenderCopy(renderer, m.map_texture, NULL, &m.map_pos);
    
    // Affichage du joueur par-dessus
    SDL_RenderCopy(renderer, m.player_texture, NULL, &m.player_pos);
}

void free_minimap(Minimap *m) {
    if (m->map_texture != NULL){
        SDL_DestroyTexture(m->map_texture);
        m->map_texture=NULL;
    }
    if (m->player_texture != NULL){
        SDL_DestroyTexture(m->player_texture);
        m->player_texture=NULL;
    }
}
