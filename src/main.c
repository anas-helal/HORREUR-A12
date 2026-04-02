#include "app.h"
#include "core.h"
#include "textures.h"
#include "audio.h"
#include "scores.h"
#include "ui.h"
#include "game.h"
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

    // Load background image first to get dimensions
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

    // Create window with correct dimensions
    app.win.win = SDL_CreateWindow("Sous-menu SDL2",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   app.win.win_w, app.win.win_h, 0);
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
