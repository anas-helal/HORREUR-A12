#include "scores.h"
#include <stdio.h>
#include <string.h>

int scores_load(App *app) {
    FILE *f = fopen("scores.txt", "r");
    if (!f) {
        fprintf(stderr, "Warning: couldn't open scores.txt\n");
        app->lb.count = 0;
        return 0;
    }
    
    int count = 0;
    while (count < MAX_SCORES && fscanf(f, "%49s %d", app->lb.scores[count].player, &app->lb.scores[count].score) == 2) {
        count++;
    }
    fclose(f);
    app->lb.count = count;

    // Create text textures for scores
    if (app->font && count > 0) {
        SDL_Color text_color = {255, 255, 255, 255};
        for (int i = 0; i < count; i++) {
            char score_str[100];
            snprintf(score_str, sizeof(score_str), "%s : %d", app->lb.scores[i].player, app->lb.scores[i].score);
            SDL_Surface *text_surf = TTF_RenderText_Solid(app->font, score_str, text_color);
            if (text_surf) {
                app->tex.score_text[i] = SDL_CreateTextureFromSurface(app->win.ren, text_surf);
                SDL_FreeSurface(text_surf);
            }
        }
    }

    return count;
}

void scores_render_leaderboard(App *app) {
    if (!app->show_leaderboard || !app->tex.sub) return;

    int sw, sh;
    SDL_QueryTexture(app->tex.sub, NULL, NULL, &sw, &sh);
    SDL_Rect dst = {(app->win.win_w - sw) / 2, (app->win.win_h - sh) / 2, sw, sh};
    SDL_RenderCopy(app->win.ren, app->tex.sub, NULL, &dst);
    
    // render score texts on leaderboard (centered)
    if (app->lb.count > 0) {
        int score_start_y = dst.y + 170;
        int score_spacing = 40;
        for (int i = 0; i < app->lb.count && i < 5; i++) {
            if (app->tex.score_text[i]) {
                int tw, th;
                SDL_QueryTexture(app->tex.score_text[i], NULL, NULL, &tw, &th);
                SDL_Rect text_rect = {
                    dst.x + (sw - tw) / 2,
                    score_start_y + i * score_spacing,
                    tw, th
                };
                SDL_RenderCopy(app->win.ren, app->tex.score_text[i], NULL, &text_rect);
            }
        }
    }
}
