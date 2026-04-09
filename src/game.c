#include "game.h"
#include "ui.h"
#include "scores.h"
#include "perso.h"
#include <stdio.h>
#include "tictactoe.h"

// Blocking text input dialog to get player name (returns 1 if saved, 0 cancelled)
static int prompt_player_name_and_save(App *app) {
    const int W = 480, H = 140;
    SDL_Window *w = SDL_CreateWindow("Enter name",
                                     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     W, H, 0);
    if (!w) return 0;
    SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!r) { SDL_DestroyWindow(w); return 0; }

    char name[50] = {0};
    int name_len = 0;
    app->current_player[0] = '\0';
    app->current_player_index = -1;
    SDL_StartTextInput();
    int running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = 0; break; }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) { name_len = 0; running = 0; break; }
                if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) { running = 0; break; }
                if (e.key.keysym.sym == SDLK_BACKSPACE && name_len > 0) { name[--name_len] = '\0'; }
            } else if (e.type == SDL_TEXTINPUT) {
                // ignore spaces
                for (int i = 0; e.text.text[i] != '\0' && name_len < (int)sizeof(name)-1; i++) {
                    char c = e.text.text[i];
                    if (c == ' ') continue;
                    name[name_len++] = c; name[name_len] = '\0';
                }
            }
        }

        SDL_SetRenderDrawColor(r, 20,20,20,255);
        SDL_RenderClear(r);
        // render prompt
        if (app->font) {
            SDL_Color white = {255,255,255,255};
            SDL_Surface *s1 = TTF_RenderText_Solid(app->font, "Enter your name (no spaces):", white);
            if (s1) {
                SDL_Texture *t1 = SDL_CreateTextureFromSurface(r, s1);
                SDL_Rect d1 = {12, 8, s1->w, s1->h};
                SDL_RenderCopy(r, t1, NULL, &d1);
                SDL_FreeSurface(s1); SDL_DestroyTexture(t1);
            }
            SDL_Surface *s2 = TTF_RenderText_Solid(app->font, name, white);
            if (s2) {
                SDL_Texture *t2 = SDL_CreateTextureFromSurface(r, s2);
                SDL_Rect d2 = {12, 40, s2->w, s2->h};
                SDL_RenderCopy(r, t2, NULL, &d2);
                SDL_FreeSurface(s2); SDL_DestroyTexture(t2);
            }
        }
        SDL_RenderPresent(r);
        SDL_Delay(16);
    }
    SDL_StopTextInput();

    // if name non-empty, append to scores.txt and reload leaderboard
    int saved = 0;
    if (name_len > 0) {
        FILE *f = fopen("scores.txt", "a");
        if (f) {
            fprintf(f, "%s %d\n", name, 0);
            fclose(f);
            scores_load(app);
            // find index of added player
            for (int i = 0; i < app->lb.count; i++) {
                if (strncmp(app->lb.scores[i].player, name, 50) == 0) {
                    strncpy(app->current_player, app->lb.scores[i].player, sizeof(app->current_player)-1);
                    app->current_player[sizeof(app->current_player)-1] = '\0';
                    app->current_player_index = i;
                    break;
                }
            }
            saved = 1;
        }
    }

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    return saved;
}




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
    int ih = (available_h - spacing * (4 - 1)) / 4;
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
    
    int total_h = 4 * item_used_h + (4 - 1) * spacing;
    int start_y = dst_y + padding + (available_h - total_h) / 2 - (int)(sh * 0.0010f);
    int bottom_limit = dst_y + sh - padding - total_h;
    if (start_y > bottom_limit) start_y = bottom_limit;
    if (start_y < dst_y + padding) start_y = dst_y + padding;
    
    for (int i = 0; i < 4; i++) {
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
                } else {
                    perso_handle_event(app, &e);
                }
        } else if (e.type == SDL_KEYUP) {
            perso_handle_event(app, &e);
        } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mx = e.button.x;
            int my = e.button.y;

            // If spritesheet overlay is shown, any click hides it
            if (app->show_spritesheet) {
                app->show_spritesheet = false;
                continue;
            }

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
                for (int i = 0; i < 4; i++) {
                    if (mx >= app->ui.items[i].x && mx <= app->ui.items[i].x + app->ui.items[i].w &&
                        my >= app->ui.items[i].y && my <= app->ui.items[i].y + app->ui.items[i].h) {
                        if (i == 0) {
                            fprintf(stdout, "Clicked: Play\n");
                            // ask for player name and save score entry before starting
                            int saved = prompt_player_name_and_save(app);
                            if (saved) {
                                app->char_visible = true;
                                perso_spawn(app);
                            }
                            // ensure submenu closes regardless
                            app->show_sub = false;
                        } else if (i == 1) {
                            fprintf(stdout, "Clicked: Leaderboard\n");
                            app->show_leaderboard = true;
                            app->show_sub = false;
                        } else if (i == 2) {
                            fprintf(stdout, "Clicked: Settings\n");
                        } else if (i == 3) {
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

    // update timing for movement
    unsigned int now = SDL_GetTicks();
    float dt = 0.0f;
    if (app->last_tick == 0) {
        app->last_tick = now;
    } else {
        dt = (now - app->last_tick) / 1000.0f;
        app->last_tick = now;
    }

    // update and render character when visible and not showing overlay
    if (!app->show_spritesheet && app->char_visible) {
        unsigned int now_anim = SDL_GetTicks();
        perso_update(app, dt, now_anim);
        perso_render(app);
    }

    // If spritesheet overlay requested, draw combined frame-by-frame animation as an overlay and return
    if (app->show_spritesheet) {
        const int rs_count = 6;
        bool any = false;
        for (int i = 0; i < rs_count; i++) if (app->tex.run_sprites[i]) any = true;
        if (any && app->anim_total_frames > 0) {
            // manual control only: no automatic advancement here

            // find which sprite and frame correspond to anim_index
            int idx = app->anim_index;
            int sprite_i = 0;
            int frame_in_sprite = 0;
            for (int i = 0; i < rs_count; i++) {
                int fc = app->run_frame_count[i];
                if (idx < fc) { sprite_i = i; frame_in_sprite = idx; break; }
                idx -= fc;
            }

            SDL_SetRenderDrawBlendMode(app->win.ren, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(app->win.ren, 0, 0, 0, 180);
            SDL_Rect overlay = {0, 0, app->win.win_w, app->win.win_h};
            SDL_RenderFillRect(app->win.ren, &overlay);

            // draw chosen frame centered
            SDL_Texture *tex = app->tex.run_sprites[sprite_i];
            int tw = 0, th = 0;
            SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
            if (tw > 0 && th > 0 && app->run_frame_count[sprite_i] > 0) {
                int frame_w = app->run_frame_w[sprite_i];
                int frame_h = app->run_frame_h[sprite_i];
                SDL_Rect src = {frame_in_sprite * frame_w, 0, frame_w, frame_h};

                int dst_w = (int)(app->win.win_w * 0.6f);
                int dst_h = (int)((double)frame_h / (double)frame_w * dst_w);
                if (dst_h > (int)(app->win.win_h * 0.7f)) {
                    dst_h = (int)(app->win.win_h * 0.7f);
                    dst_w = (int)((double)frame_w / (double)frame_h * dst_h);
                }
                SDL_Rect dst = {(app->win.win_w - dst_w) / 2, (app->win.win_h - dst_h) / 2, dst_w, dst_h};
                SDL_RenderCopy(app->win.ren, tex, &src, &dst);

                // small HUD: show frame index and controls
                if (app->font) {
                    char info[128];
                    snprintf(info, sizeof(info), "Frame %d/%d  (←/→ to step, R reset)", app->anim_index+1, app->anim_total_frames);
                    SDL_Color c = {255,255,255,255};
                    SDL_Surface *s = TTF_RenderText_Solid(app->font, info, c);
                    if (s) {
                        SDL_Texture *t = SDL_CreateTextureFromSurface(app->win.ren, s);
                        SDL_Rect r = {10, 10, s->w, s->h};
                        SDL_RenderCopy(app->win.ren, t, NULL, &r);
                        SDL_FreeSurface(s);
                        SDL_DestroyTexture(t);
                    }
                }
            }

            SDL_RenderPresent(app->win.ren);
            return;
        }
    }

    // (Bottom run sprites removed)

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
