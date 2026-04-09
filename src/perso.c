#include "perso.h"
#include "tictactoe.h"

void perso_spawn(App *app) {
    if (app->run_frame_w[0] > 0) {
        app->char_x = -(float)app->run_frame_w[0];
    } else {
        app->char_x = -64.0f;
    }
}

void perso_init(App *app) {
    app->char_facing = 1;
    app->attack_sprite_index = 0;
    app->attack_frame_index = 0;
    app->attack_last_tick = SDL_GetTicks();
    app->attack_playing = false;
}

int perso_get_run_sprite_index(App *app, int *frame_in_sprite) {
    int idx = app->anim_index;
    for (int i = 0; i < 6; i++) {
        int fc = app->run_frame_count[i];
        if (fc <= 0) fc = 1;
        if (idx < fc) {
            if (frame_in_sprite) *frame_in_sprite = idx;
            return i;
        }
        idx -= fc;
    }

    if (frame_in_sprite) *frame_in_sprite = 0;
    return 0;
}

static int first_loaded_attack_sprite(App *app) {
    for (int i = 0; i < 5; i++) {
        if (app->tex.attack_sprites[i]) {
            return i;
        }
    }

    return -1;
}

static int next_loaded_attack_sprite(App *app, int current_index) {
    for (int i = current_index + 1; i < 5; i++) {
        if (app->tex.attack_sprites[i]) {
            return i;
        }
    }

    return -1;
}

void perso_start_attack(App *app) {
    int attack_sprite = first_loaded_attack_sprite(app);
    if (attack_sprite < 0) return;

    app->attack_sprite_index = attack_sprite;
    app->attack_frame_index = 0;
    app->attack_last_tick = SDL_GetTicks();
    app->attack_playing = true;
    app->char_vx = 0.0f;
}

void perso_update_attack(App *app, unsigned int now_tick) {
    if (!app->attack_playing || app->attack_sprite_index < 0 || app->attack_sprite_index >= 5) {
        return;
    }

    int fc = app->attack_frame_count[app->attack_sprite_index];
    if (fc < 1) fc = 1;

    if (now_tick - app->attack_last_tick < (unsigned int)app->attack_frame_ms) {
        return;
    }

    if (app->attack_frame_index + 1 < fc) {
        app->attack_frame_index++;
        app->attack_last_tick = now_tick;
        return;
    }

    int next_attack_sprite = next_loaded_attack_sprite(app, app->attack_sprite_index);
    if (next_attack_sprite >= 0) {
        app->attack_sprite_index = next_attack_sprite;
        app->attack_frame_index = 0;
        app->attack_last_tick = now_tick;
        return;
    }

    app->attack_playing = false;
    app->attack_frame_index = 0;
    app->char_vx = 0.0f;
}

void perso_handle_event(App *app, const SDL_Event *e) {
    if (!e) {
        return;
    }

    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_RIGHT) {
            app->char_vx = app->char_speed;
            app->char_facing = 1;
        } else if (e->key.keysym.sym == SDLK_LEFT) {
            app->char_vx = -app->char_speed;
            app->char_facing = -1;
        } else if (e->key.keysym.sym == SDLK_r) {
            app->anim_index = 0;
        } else if (e->key.keysym.sym == SDLK_a) {
            if (app->char_visible && !app->show_spritesheet) {
                perso_start_attack(app);
            }
        }
    } else if (e->type == SDL_KEYUP) {
        if (e->key.keysym.sym == SDLK_RIGHT || e->key.keysym.sym == SDLK_LEFT) {
            app->char_vx = 0.0f;
        }
    }
}

void perso_update(App *app, float dt, unsigned int now_tick) {
    if (!app->char_visible) {
        return;
    }

    if (!app->attack_playing) {
        app->char_x += app->char_vx * dt;
    }

    if (!app->attack_playing && app->char_vx != 0.0f && app->anim_total_frames > 0) {
        if (now_tick - app->anim_last_tick >= (unsigned int)app->anim_speed_ms) {
            app->anim_index = (app->anim_index + 1) % app->anim_total_frames;
            app->anim_last_tick = now_tick;
        }
    }

    perso_update_attack(app, now_tick);

    int sprite_i = 0, frame_in_sprite = 0;
    if (app->attack_playing && app->attack_sprite_index >= 0 && app->attack_sprite_index < 5) {
        sprite_i = app->attack_sprite_index;
        frame_in_sprite = app->attack_frame_index;
    } else {
        sprite_i = perso_get_run_sprite_index(app, &frame_in_sprite);
    }

    int base_frame_w = app->run_frame_w[sprite_i] > 0 ? app->run_frame_w[sprite_i] : 64;
    int frame_w = (int)(base_frame_w * app->char_scale);
    if (app->char_x > app->win.win_w) app->char_x = -frame_w;
    if (app->char_x + frame_w < 0) app->char_x = app->win.win_w;
}

void perso_render(App *app) {
    if (!app->char_visible || app->show_spritesheet) {
        return;
    }

    int sprite_i = 0, frame_in_sprite = 0;
    if (app->attack_playing && app->attack_sprite_index >= 0 && app->attack_sprite_index < 5) {
        sprite_i = app->attack_sprite_index;
        frame_in_sprite = app->attack_frame_index;
    } else {
        sprite_i = perso_get_run_sprite_index(app, &frame_in_sprite);
    }

    SDL_Texture *tex = app->attack_playing ? app->tex.attack_sprites[sprite_i] : app->tex.run_sprites[sprite_i];
    if (!tex) {
        return;
    }

    int source_w = app->attack_playing ? app->attack_frame_w[sprite_i] : app->run_frame_w[sprite_i];
    int source_h = app->attack_playing ? app->attack_frame_h[sprite_i] : app->run_frame_h[sprite_i];
    if (source_w <= 0) source_w = 64;
    if (source_h <= 0) source_h = 64;

    SDL_Rect src = {frame_in_sprite * source_w, 0, source_w, source_h};
    SDL_Rect dst = {(int)app->char_x, app->char_y, (int)(source_w * app->char_scale), (int)(source_h * app->char_scale)};
    if (dst.w > app->win.win_w / 2) {
        float s = (float)(app->win.win_w / 2) / (float)dst.w;
        dst.w = (int)(dst.w * s);
        dst.h = (int)(dst.h * s);
        dst.y = app->win.win_h - dst.h - 10;
    }

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (app->char_facing < 0) flip = SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(app->win.ren, tex, &src, &dst, 0.0, NULL, flip);

    if (app->tex.balloon) {
        int bw = 0, bh = 0;
        SDL_QueryTexture(app->tex.balloon, NULL, NULL, &bw, &bh);
        if (bw > 0 && bh > 0) {
            int bdw = (int)(bw * app->char_scale * app->balloon_scale);
            int bdh = (int)((double)bh / (double)bw * bdw);
            int bx = (int)app->balloon_x - bdw / 2;
            int by = (int)app->balloon_y - bdh / 2;
            SDL_Rect bdst = {bx, by, bdw, bdh};
            SDL_RenderCopy(app->win.ren, app->tex.balloon, NULL, &bdst);
            if (!app->balloon_triggered) {
                SDL_Rect crect = dst;
                if (SDL_HasIntersection(&crect, &bdst)) {
                    app->balloon_triggered = true;
                    tictactoe_run(app);
                    app->char_vx = 0.0f;
                }
            }
        }
    }
}