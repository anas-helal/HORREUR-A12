#include "textures.h"
#include <stdio.h>

int textures_load(App *app) {
    // Load background
    SDL_Surface *bg_surf = IMG_Load("assets/background_jeuvideo.png");
    if (bg_surf) {
        app->tex.bg = SDL_CreateTextureFromSurface(app->win.ren, bg_surf);
        SDL_FreeSurface(bg_surf);
    } else {
        fprintf(stderr, "Warning: couldn't load background_jeuvideo.png: %s\n", IMG_GetError());
    }

    // Load button textures
    app->tex.btn = IMG_LoadTexture(app->win.ren, "assets/bouton1.png");
    if (!app->tex.btn) app->tex.btn = IMG_LoadTexture(app->win.ren, "assets/button.png");
    
    app->tex.btn_hover = IMG_LoadTexture(app->win.ren, "assets/boutonHover.png");
    if (!app->tex.btn_hover) app->tex.btn_hover = IMG_LoadTexture(app->win.ren, "assets/button_hover.png");

    // Load submenu texture
    app->tex.sub = IMG_LoadTexture(app->win.ren, "assets/sousmenu.png");

    // Load font
    app->font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 20);
    if (!app->font) {
        fprintf(stderr, "Warning: couldn't load font: %s\n", TTF_GetError());
    }

    // Create text textures for main button
    if (app->font) {
        SDL_Color text_color = {255, 255, 255, 255};
        
        SDL_Surface *text_surf_menu = TTF_RenderText_Solid(app->font, "sous menu", text_color);
        if (text_surf_menu) {
            app->tex.btn_text_menu = SDL_CreateTextureFromSurface(app->win.ren, text_surf_menu);
            SDL_FreeSurface(text_surf_menu);
        }
        
        SDL_Surface *text_surf_return = TTF_RenderText_Solid(app->font, "retour", text_color);
        if (text_surf_return) {
            app->tex.btn_text_return = SDL_CreateTextureFromSurface(app->win.ren, text_surf_return);
            SDL_FreeSurface(text_surf_return);
        }
    }

    return 1;
}

void textures_cleanup(App *app) {
    if (app->tex.bg) SDL_DestroyTexture(app->tex.bg);
    if (app->tex.btn) SDL_DestroyTexture(app->tex.btn);
    if (app->tex.btn_hover) SDL_DestroyTexture(app->tex.btn_hover);
    if (app->tex.sub) SDL_DestroyTexture(app->tex.sub);
    if (app->tex.item) SDL_DestroyTexture(app->tex.item);
    if (app->tex.btn_text_menu) SDL_DestroyTexture(app->tex.btn_text_menu);
    if (app->tex.btn_text_return) SDL_DestroyTexture(app->tex.btn_text_return);
    
    for (int i = 0; i < 3; i++) {
        if (app->tex.item_text[i]) SDL_DestroyTexture(app->tex.item_text[i]);
    }
    
    for (int i = 0; i < MAX_SCORES; i++) {
        if (app->tex.score_text[i]) SDL_DestroyTexture(app->tex.score_text[i]);
    }
}
