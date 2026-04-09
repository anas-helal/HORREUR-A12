#include "app.h"
#include "core.h"
#include "textures.h"
#include "audio.h"
#include "scores.h"
#include "ui.h"
#include "game.h"
#include "perso.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    App app;
    memset(&app, 0, sizeof(app));
    app.running = true;

    // Initialize core SDL
    if (!core_init(&app)) {
        fprintf(stderr, "Failed to initialize core\n");
        return 1;
    }

    // Load background image first to get its dimensions and create window at same size
    app.win.win_w = 800;
    app.win.win_h = 600;
    SDL_Surface *bg_surf = IMG_Load("assets/background_jeuvideo.png");
    if (bg_surf) {
        app.win.win_w = bg_surf->w;
        app.win.win_h = bg_surf->h;
        SDL_FreeSurface(bg_surf);
    } else {
        fprintf(stderr, "Warning: couldn't load background, using defaults\n");
    }

    Uint32 win_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    app.win.win = SDL_CreateWindow("Sous-menu SDL2",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   app.win.win_w, app.win.win_h,
                                   win_flags);
    if (!app.win.win) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        core_cleanup(&app);
        return 1;
    }

    // Create renderer
    app.win.ren = SDL_CreateRenderer(app.win.win, -1,
                                     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!app.win.ren) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(app.win.win);
        core_cleanup(&app);
        return 1;
    }

    // Load textures
    if (!textures_load(&app)) {
        fprintf(stderr, "Failed to load textures\n");
    }

    // Initialize character position and timing
    app.last_tick = SDL_GetTicks();
    app.char_x = 0.0f;
    app.char_speed = 120.0f; // pixels per second
    app.char_vx = 0.0f;
    app.char_visible = false; // start hidden until Play clicked
    app.char_scale = 2.0f; // default make character larger
        app.char_y_offset = -10; // default vertical offset (px) — negative moves character lower
        // fixed balloon position (world coordinates)
        app.balloon_x = app.win.win_w / 2;
        app.balloon_y = app.win.win_h * 0.92f; // place balloon lower (92% down)
        app.balloon_scale = 0.4f; // make balloon smaller (adjustable)
        app.balloon_triggered = false;
        // second balloon: place to the left and at same vertical level as the first
        app.balloon2_x = app.win.win_w * 0.25f;
        app.balloon2_y = app.balloon_y;
        app.balloon2_scale = 0.35f;
        app.balloon2_triggered = false;
        perso_init(&app);
    // place character a bit higher than the bottom
    if (app.run_frame_h[0] > 0) {
        int ch = (int)(app.run_frame_h[0] * app.char_scale);
        app.char_y = app.win.win_h - ch - 10 - app.char_y_offset; // apply adjustable offset
    } else {
        int ch = (int)(64 * app.char_scale);
        app.char_y = app.win.win_h - ch - 10 - app.char_y_offset;
    }

    // Load audio
    if (!audio_init(&app)) {
        fprintf(stderr, "Failed to initialize audio\n");
    }

    // Load scores
    if (scores_load(&app) == 0) {
        fprintf(stderr, "Failed to load scores\n");
    }

    // Initialize UI
    if (!ui_init(&app)) {
        fprintf(stderr, "Failed to initialize UI\n");
    }

    // Run game loop
    game_run(&app);

    // Cleanup
    textures_cleanup(&app);
    audio_cleanup(&app);
    SDL_DestroyRenderer(app.win.ren);
    SDL_DestroyWindow(app.win.win);
    core_cleanup(&app);

    return 0;
}
