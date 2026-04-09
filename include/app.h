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
    SDL_Texture *item_text[4];
    SDL_Texture *run_sprites[6];
    SDL_Texture *attack_sprites[5];
    SDL_Texture *balloon;
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
    SDL_Rect items[4];
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
    bool item_hover[4];
    bool btn_hover;
    // run sprites animation state
    int run_frame_index[6];
    int run_frame_count[6];
    int run_frame_w[6];
    int run_frame_h[6];
    unsigned int run_last_tick;
    int run_frame_ms;
    int attack_frame_count[5];
    int attack_frame_w[5];
    int attack_frame_h[5];
    int attack_sprite_index;
    int attack_frame_index;
    unsigned int attack_last_tick;
    int attack_frame_ms;
    bool attack_playing;
    // character movement
    float char_x;
    int char_y;
    float char_speed; // pixels per second
    float char_vx; // current velocity
    int char_facing;
    bool char_visible;
    unsigned int last_tick;
    float char_scale; // drawing scale for character
    int char_y_offset; // vertical offset from bottom (positive moves character up)
    // fixed balloon position (world coordinates)
    float balloon_x;
    float balloon_y;
    float balloon_scale; // multiplier for balloon size
    bool balloon_triggered; // set when collision puzzle has been shown
    // combined animation state (frames across all run sprites)
    bool show_spritesheet;
    bool anim_playing;
    int anim_index;
    int anim_total_frames;
    unsigned int anim_last_tick;
    int anim_speed_ms;
    // current player info for scoring
    char current_player[50];
    int current_player_index;
} App;

#endif
