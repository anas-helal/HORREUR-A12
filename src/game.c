#include "game.h"
#include "ui.h"
#include "scores.h"
#include <stdio.h>

static void compute_submenu_positions(App *app) {
    if (!app->tex.sub) return;

    int sw, sh;
    SDL_QueryTexture(app->tex.sub, NULL, NULL, &sw, &sh);
    int dst_x = (app->win.win_w - sw) / 2;
    int dst_y = (app->win.win_h - sh) / 2;
    int padding = sw / 20;
    if (padding < 8) padding = 8;
    int spacing = padding / 2;
    int available_h = sh - 2 * padding;
    
    float item_scale = 0.60f;
    int ih = (available_h - spacing * (3 - 1)) / 3;
    ih = (int)(ih * item_scale);
    if (ih < 8) ih = app->win.win_h / 15;
    int iw = (int)((sw - 2 * padding) * item_scale);
    if (iw < app->win.win_w / 6) iw = app->win.win_w / 6;
    
    int item_used_w = iw;
    int item_used_h = ih;
    if (app->tex.item) {
        int t_w, t_h;
        SDL_QueryTexture(app->tex.item, NULL, NULL, &t_w, &t_h);
        if (t_w > 0 && t_h > 0) {
            float scale = (float)ih / (float)t_h;
            int w = (int)(t_w * scale);
            int h = (int)(t_h * scale);
            if (w > iw) {
                scale = (float)iw / (float)t_w;
                w = iw;
                h = (int)(t_h * scale);
            }
            item_used_w = w;
            item_used_h = h;
        }
    }
    
    int total_h = 3 * item_used_h + (3 - 1) * spacing;
    int start_y = dst_y + padding + (available_h - total_h) / 2 - (int)(sh * 0.0010f);
    int bottom_limit = dst_y + sh - padding - total_h;
    if (start_y > bottom_limit) start_y = bottom_limit;
    if (start_y < dst_y + padding) start_y = dst_y + padding;
    
    for (int i = 0; i < 3; i++) {
        app->ui.items[i].w = item_used_w;
        app->ui.items[i].h = item_used_h;
        app->ui.items[i].x = dst_x + (sw - item_used_w) / 2;
        app->ui.items[i].y = start_y + i * (item_used_h + spacing);
    }
}

void game_handle_events(App *app) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            app->running = false;
        } else if (e.type == SDL_MOUSEMOTION) {
            app->mouse_x = e.motion.x;
            app->mouse_y = e.motion.y;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_e) {
                app->show_leaderboard = !app->show_leaderboard;
                app->show_sub = false;
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mx = e.button.x;
            int my = e.button.y;
            
            if (mx >= app->ui.btn.x && mx <= app->ui.btn.x + app->ui.btn.w &&
                my >= app->ui.btn.y && my <= app->ui.btn.y + app->ui.btn.h) {
                app->show_sub = !app->show_sub;
                if (app->show_sub && !app->tex.item) {
                    app->tex.item = IMG_LoadTexture(app->win.ren, "assets/bouton1.png");
                    if (!app->tex.item) fprintf(stderr, "Warning: couldn't load assets/bouton1.png\n");
                }
                if (app->show_sub) {
                    compute_submenu_positions(app);
                }
            }
            
            // clicks on submenu items
            if (app->show_sub && !app->show_leaderboard) {
                for (int i = 0; i < 3; i++) {
                    if (mx >= app->ui.items[i].x && mx <= app->ui.items[i].x + app->ui.items[i].w &&
                        my >= app->ui.items[i].y && my <= app->ui.items[i].y + app->ui.items[i].h) {
                        if (i == 0) {
                            fprintf(stdout, "Clicked: Leaderboard\n");
                            app->show_leaderboard = true;
                            app->show_sub = false;
                        } else if (i == 1) {
                            fprintf(stdout, "Clicked: Settings\n");
                        } else if (i == 2) {
                            app->running = false;
                        }
                    }
                }
            }
            
            // handle leaderboard back button
            if (app->show_leaderboard) {
                if (mx >= app->ui.btn.x && mx <= app->ui.btn.x + app->ui.btn.w &&
                    my >= app->ui.btn.y && my <= app->ui.btn.y + app->ui.btn.h) {
                    app->show_leaderboard = false;
                }
            }
        }
    }
}

void game_render(App *app) {
    SDL_SetRenderDrawColor(app->win.ren, 0, 0, 0, 255);
    SDL_RenderClear(app->win.ren);

    if (app->tex.bg) {
        SDL_Rect dst = {0, 0, app->win.win_w, app->win.win_h};
        SDL_RenderCopy(app->win.ren, app->tex.bg, NULL, &dst);
    }

    ui_update_hover(app);
    ui_render_main_button(app);
    ui_render_submenu(app);
    scores_render_leaderboard(app);

    SDL_RenderPresent(app->win.ren);
}

void game_run(App *app) {
    while (app->running) {
        game_handle_events(app);
        game_render(app);
        SDL_Delay(16);
    }
}
