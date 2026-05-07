#ifndef MINIMAP_H
#define MINIMAP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
/**
* @struct minimap
* @brief struct for minimap
*/
typedef struct {
    SDL_Texture* Img[2];
    SDL_Texture* texteTexture;
    SDL_Rect Pos;
    SDL_Rect posTexte;
    int num; 
} Boutton;
typedef struct {
    SDL_Texture *image_bg;
    SDL_Rect pos_bg;
    int nbr_boutton;
    Boutton tab[10];
    Mix_Music *musique;
    Mix_Chunk *son; 
    TTF_Font *font;
    int num_bt_actif;
} SousMenu;
typedef struct {
    SDL_Texture *map_texture;
    SDL_Texture *player_texture;
    SDL_Rect map_pos;/*!< Rectangle>*/
    SDL_Rect player_pos;
} Minimap;
typedef struct {
    SDL_Texture *texture;
    SDL_Rect pos;
} Entite;
void init_minimap(Minimap *m, SDL_Renderer *renderer);
void MAJminimap(SDL_Rect posJoueur,Minimap *m,SDL_Rect camera,int redimensionnement);
void afficher(Minimap m, SDL_Renderer *renderer);
int collision(SDL_Rect a, SDL_Rect b);
void free_minimap(Minimap *m);
SousMenu init_SousMenu(SDL_Renderer *renderer);
void afficher_SousMenu(SousMenu M, SDL_Renderer *renderer);
int update_SousMenu(SousMenu *M, int input);
void liberer_SousMenu(SousMenu *M);
void sauvegarder_jeu(int score, int vies, SDL_Rect joueur, SDL_Rect camera);
int charger_jeu(int *score, int *vies, SDL_Rect *joueur, SDL_Rect *camera);
#endif

