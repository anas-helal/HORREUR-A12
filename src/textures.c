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

    // Load run spritesheets
    const char *run_files[6] = {
        "assets/run1_spritesheet-.png",
        "assets/run2_spritesheet-.png",
        "assets/run3_spritesheet-.png",
        "assets/run4_spritesheet-.png",
        "assets/run5_spritesheet-.png",
        "assets/run6_spritesheet-.png"
    };
    for (int i = 0; i < 6; i++) {
        app->tex.run_sprites[i] = IMG_LoadTexture(app->win.ren, run_files[i]);
        if (!app->tex.run_sprites[i]) {
            fprintf(stderr, "Warning: couldn't load %s: %s\n", run_files[i], IMG_GetError());
        }
        else {
            int tw, th;
            SDL_QueryTexture(app->tex.run_sprites[i], NULL, NULL, &tw, &th);
            if (th > 0) {
                int fc = tw / th;
                if (fc < 1) fc = 1;
                app->run_frame_count[i] = fc;
                app->run_frame_w[i] = tw / fc;
                app->run_frame_h[i] = th;
            } else {
                app->run_frame_count[i] = 1;
                app->run_frame_w[i] = tw;
                app->run_frame_h[i] = th;
            }
            app->run_frame_index[i] = 0;
        }
    }
    app->run_frame_ms = 100;
    app->run_last_tick = SDL_GetTicks();

    const char *attack_files[5] = {
        "assets/spritesheet_attack_perso1.png",
        "assets/spritesheet_attack_perso2.png",
        "assets/spritesheet_attack_perso3.png",
        "assets/spritesheet_attack_perso4.png",
        "assets/spritesheet_attack_perso5.png"
    };
    for (int i = 0; i < 5; i++) {
        app->tex.attack_sprites[i] = IMG_LoadTexture(app->win.ren, attack_files[i]);
        if (!app->tex.attack_sprites[i]) {
            fprintf(stderr, "Warning: couldn't load %s: %s\n", attack_files[i], IMG_GetError());
        } else {
            int tw, th;
            SDL_QueryTexture(app->tex.attack_sprites[i], NULL, NULL, &tw, &th);
            if (th > 0) {
                int fc = tw / th;
                if (fc < 1) fc = 1;
                app->attack_frame_count[i] = fc;
                app->attack_frame_w[i] = tw / fc;
                app->attack_frame_h[i] = th;
            } else {
                app->attack_frame_count[i] = 1;
                app->attack_frame_w[i] = tw;
                app->attack_frame_h[i] = th;
            }
        }
    }
    app->attack_frame_ms = 140;
    app->attack_last_tick = SDL_GetTicks();
    app->attack_sprite_index = 0;
    app->attack_frame_index = 0;
    app->attack_playing = false;

    // Initialize combined animation state: total frames across all run sprites
    app->anim_total_frames = 0;
    for (int i = 0; i < 6; i++) app->anim_total_frames += app->run_frame_count[i];
    app->anim_index = 0;
    app->anim_speed_ms = 100;
    app->anim_last_tick = SDL_GetTicks();
    // start with manual control (no automatic playback)
    app->anim_playing = 0;
    app->show_spritesheet = 0;

    // Load balloon texture (optional)
    app->tex.balloon = IMG_LoadTexture(app->win.ren, "assets/ballon.png");
    if (!app->tex.balloon) {
        // not fatal, just warn
        fprintf(stderr, "Info: couldn't load assets/ballon.png: %s\n", IMG_GetError());
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
    if (app->tex.balloon) SDL_DestroyTexture(app->tex.balloon);
    
    for (int i = 0; i < 4; i++) {
        if (app->tex.item_text[i]) SDL_DestroyTexture(app->tex.item_text[i]);
    }

    for (int i = 0; i < 6; i++) {
        if (app->tex.run_sprites[i]) SDL_DestroyTexture(app->tex.run_sprites[i]);
    }

    for (int i = 0; i < 5; i++) {
        if (app->tex.attack_sprites[i]) SDL_DestroyTexture(app->tex.attack_sprites[i]);
    }
    
    for (int i = 0; i < MAX_SCORES; i++) {
        if (app->tex.score_text[i]) SDL_DestroyTexture(app->tex.score_text[i]);
    }
}
