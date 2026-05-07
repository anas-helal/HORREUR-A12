/**
* @file background.c
*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include "minimap.h"
/**
* @brief To initialize the minimap b .
* @param b the background
* @param url the url of the image
* @return Nothing
*/
void init_minimap(Minimap *m, SDL_Renderer *renderer) {
    m->map_texture = IMG_LoadTexture(renderer, "house_haunted.jpg");
    m->player_texture = IMG_LoadTexture(renderer, "ball.png");
    m->map_pos.x = 850; 
    m->map_pos.y = 20;
    m->map_pos.w = 200;
    m->map_pos.h = 120;
    m->player_pos.w = 15;
    m->player_pos.h = 15;
}
/**
* @brief To update the position of player b .
* @param scren the screen
* @param b the background
* @return Nothing
*/
void MAJminimap(SDL_Rect posJoueur,Minimap *m,SDL_Rect camera,int redimensionnement) {
    SDL_Rect posJoueurABS;
    posJoueurABS.x=posJoueur.x+camera.x;
    posJoueurABS.y=posJoueur.y+camera.y;
    m->player_pos.x=m->map_pos.x+(posJoueurABS.x*redimensionnement)/100;
    m->player_pos.y=m->map_pos.y+(posJoueurABS.y*redimensionnement)/100;
}
int collision(SDL_Rect a, SDL_Rect b) {
    if ((a.x + a.w < b.x) || (a.x > b.x + b.w) || 
        (a.y + a.h < b.y) || (a.y > b.y + b.h)) {
        return 0;
    } else {
        return 1;
    }
}
/**
* @brief To show the minimap b .
* @param scren the screen
* @param b the minimaps
* @return Nothing
*/
void afficher(Minimap m, SDL_Renderer *renderer) {
    SDL_RenderCopy(renderer, m.map_texture, NULL, &m.map_pos);
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
SousMenu init_SousMenu(SDL_Renderer *renderer) {
    SousMenu M;
    M.image_bg = IMG_LoadTexture(renderer, "house_haunted.jpg");
    M.pos_bg = (SDL_Rect){340, 210, 400, 300};
    M.nbr_boutton = 2;
    M.num_bt_actif = 1;
    M.son = Mix_LoadWAV("hover_sound.mp3");
    M.musique = Mix_LoadMUS("pause_music.mp3");
    M.tab[0].Img[0] = IMG_LoadTexture(renderer, "bouton_rouge.png");
    M.tab[0].Img[1] = IMG_LoadTexture(renderer, "bouton_vert.png");
    M.tab[0].Pos = (SDL_Rect){440, 300, 200, 60};
    M.tab[0].num = 1;
    M.tab[1].Img[0] = IMG_LoadTexture(renderer, "bouton_rouge.png");
    M.tab[1].Img[1] = IMG_LoadTexture(renderer, "bouton_vert.png");
    M.tab[1].Pos = (SDL_Rect){440, 380, 200, 60};
    M.tab[1].num = 2;
    return M;
}
void afficher_SousMenu(SousMenu M, SDL_Renderer *renderer) {
    SDL_RenderCopy(renderer, M.image_bg, NULL, &M.pos_bg);
    for(int i = 0; i < M.nbr_boutton; i++) {
        int etat = (M.tab[i].num == M.num_bt_actif) ? 1 : 0;
        SDL_RenderCopy(renderer, M.tab[i].Img[etat], NULL, &M.tab[i].Pos);
    }
}
int update_SousMenu(SousMenu *M, int input) {
    int ancien_actif = M->num_bt_actif;
    if (input == 2) {
        if (M->num_bt_actif < M->nbr_boutton) M->num_bt_actif++;
    } else if (input == 1) {
        if (M->num_bt_actif > 1) M->num_bt_actif--;
    }
    if (ancien_actif != M->num_bt_actif && M->son) {
        Mix_PlayChannel(-1, M->son, 0);
    }
    if (input == 3) {
        if (M->num_bt_actif == 1) return -1;
        return 3;
    }
    return 4;
}
void sauvegarder_jeu(int score, int vies, SDL_Rect joueur, SDL_Rect camera) {
    FILE *f = fopen("sauvegarde.bin", "wb");
    if (f != NULL) {
        fwrite(&score, sizeof(int), 1, f);
        fwrite(&vies, sizeof(int), 1, f);
        fwrite(&joueur, sizeof(SDL_Rect), 1, f);
        fwrite(&camera, sizeof(SDL_Rect), 1, f);
        fclose(f);
        printf("Partie sauvegardee avec succes.\n");
    }
}
int charger_jeu(int *score, int *vies, SDL_Rect *joueur, SDL_Rect *camera) {
    FILE *f = fopen("sauvegarde.bin", "rb");
    if (f != NULL) {
        fread(score, sizeof(int), 1, f);
        fread(vies, sizeof(int), 1, f);
        fread(joueur, sizeof(SDL_Rect), 1, f);
        fread(camera, sizeof(SDL_Rect), 1, f);
        fclose(f);
        return 1;
    }
    return 0;
}
void liberer_SousMenu(SousMenu *M) {
    SDL_DestroyTexture(M->image_bg);
    for(int i = 0; i < M->nbr_boutton; i++) {
        SDL_DestroyTexture(M->tab[i].Img[0]);
        SDL_DestroyTexture(M->tab[i].Img[1]);
    }
    Mix_FreeChunk(M->son);
    Mix_FreeMusic(M->musique);
}
/**
* @file source.c
* @brief Testing Program.
* @author C Team
* @version 0.1
* @date May 03, 2026
*
* Testing program for background scrollilng
*
*/
