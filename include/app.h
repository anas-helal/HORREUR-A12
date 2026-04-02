#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#define MAX_SCORES 10

typedef struct {
    char player[50];
    int score;
} Score;

typedef struct {
    SDL_Window *win;
    SDL_Renderer *ren;
    int win_w;
    int win_h;
} Window;

typedef struct {
    SDL_Texture *bg;
    SDL_Texture *btn;
    SDL_Texture *btn_hover;
    SDL_Texture *sub;
    SDL_Texture *item;
    SDL_Texture *btn_text_menu;
    SDL_Texture *btn_text_return;
    SDL_Texture *item_text[3];
    SDL_Texture *score_text[MAX_SCORES];
} Textures;

typedef struct {
    Mix_Music *bgm;
} Audio;

typedef struct {
    Score scores[MAX_SCORES];
    int count;
} Leaderboard;

typedef struct {
    SDL_Rect btn;
    SDL_Rect items[3];
} UIRects;

typedef struct {
    Window win;
    Textures tex;
    Audio audio;
    Leaderboard lb;
    UIRects ui;
    TTF_Font *font;
    
    bool running;
    bool show_sub;
    bool show_leaderboard;
    int mouse_x;
    int mouse_y;
    bool item_hover[3];
    bool btn_hover;
} App;

#endif
