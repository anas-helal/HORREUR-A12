#include "quiz.h"
#include "app.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "scores.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char q[512];
    char a[256];
    char b[256];
    char c[256];
    char correct; // 'A' 'B' or 'C'
} QuizItem;

static int load_quiz_items(const char *path, QuizItem **out_items) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[1024];
    QuizItem *items = NULL;
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        // skip empty or comment lines
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\n' || *p == '\0' || *p == '#') continue;

        // remove trailing newline
        char *nl = strchr(line, '\n'); if (nl) *nl = '\0';

        // tokenize by '|'
        char *parts[5] = {0};
        int pi = 0;
        char *tok = strtok(line, "|");
        while (tok && pi < 5) { parts[pi++] = tok; tok = strtok(NULL, "|"); }
        if (pi < 5) continue; // malformed

        QuizItem it;
        strncpy(it.q, parts[0], sizeof(it.q)-1); it.q[sizeof(it.q)-1] = '\0';
        strncpy(it.a, parts[1], sizeof(it.a)-1); it.a[sizeof(it.a)-1] = '\0';
        strncpy(it.b, parts[2], sizeof(it.b)-1); it.b[sizeof(it.b)-1] = '\0';
        strncpy(it.c, parts[3], sizeof(it.c)-1); it.c[sizeof(it.c)-1] = '\0';
        char corr = 'A';
        if (parts[4] && parts[4][0]) corr = (char)toupper((unsigned char)parts[4][0]);
        it.correct = corr;

        QuizItem *tmp = realloc(items, sizeof(QuizItem) * (count + 1));
        if (!tmp) { free(items); fclose(f); return 0; }
        items = tmp;
        items[count++] = it;
    }
    fclose(f);
    *out_items = items;
    return count;
}

void quiz_show(App *app) {
    QuizItem *items = NULL;
    int n = load_quiz_items("assets/quiz.txt", &items);

    SDL_Surface *surf = IMG_Load("assets/quiz_frame.png");
    if (!surf) {
        fprintf(stderr, "Info: couldn't load assets/quiz_frame.png: %s\n", IMG_GetError());
        if (items) free(items);
        return;
    }

    int orig_w = surf->w;
    int orig_h = surf->h;

    SDL_Rect disp;
    if (SDL_GetDisplayBounds(0, &disp) != 0) { disp.w = orig_w; disp.h = orig_h; }

    float max_frac = 0.80f;
    float min_frac = 0.50f;
    float scale = 1.0f;
    if (orig_w > (int)(disp.w * max_frac) || orig_h > (int)(disp.h * max_frac)) {
        float sx = (disp.w * max_frac) / (float)orig_w;
        float sy = (disp.h * max_frac) / (float)orig_h;
        scale = sx < sy ? sx : sy;
    } else if (orig_w < (int)(disp.w * min_frac) && orig_h < (int)(disp.h * min_frac)) {
        float sx = (disp.w * min_frac) / (float)orig_w;
        float sy = (disp.h * min_frac) / (float)orig_h;
        scale = sx < sy ? sx : sy;
        if (scale > 2.0f) scale = 2.0f;
    }

    int target_w = (int)(orig_w * scale);
    int target_h = (int)(orig_h * scale);
    if (target_w < 200) target_w = 200;
    if (target_h < 150) target_h = 150;

    SDL_Window *w = SDL_CreateWindow("Quiz", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, target_w, target_h, 0);
    if (!w) { SDL_FreeSurface(surf); if (items) free(items); return; }
    SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!r) { SDL_DestroyWindow(w); SDL_FreeSurface(surf); if (items) free(items); return; }

    SDL_Texture *bg = SDL_CreateTextureFromSurface(r, surf);
    SDL_FreeSurface(surf);
    if (!bg) { SDL_DestroyRenderer(r); SDL_DestroyWindow(w); if (items) free(items); return; }

    // If no items, just show background and wait for click/ESC
    if (n <= 0) {
        int running = 1;
        while (running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) { running = 0; break; }
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) { running = 0; break; }
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) { running = 0; break; }
            }
            SDL_SetRenderDrawColor(r, 0,0,0,255);
            SDL_RenderClear(r);
            SDL_Rect dst = {0,0,target_w,target_h};
            SDL_RenderCopy(r, bg, NULL, &dst);
            SDL_RenderPresent(r);
            SDL_Delay(16);
        }
        SDL_DestroyTexture(bg);
        SDL_DestroyRenderer(r);
        SDL_DestroyWindow(w);
        if (items) free(items);
        return;
    }

    srand((unsigned int)SDL_GetTicks());
    int idx = rand() % n;
    QuizItem q = items[idx];

    // Prepare text textures using app->font if available
    TTF_Font *font = app ? app->font : NULL;
    SDL_Color white = {255,255,255,255};
    SDL_Texture *qtex = NULL, *atex = NULL, *btex = NULL, *ctex = NULL;
    SDL_Rect qrect = {20, 30, target_w - 40, 0};
    SDL_Rect arect, brect, crect;

    if (font) {
        SDL_Surface *s = TTF_RenderText_Blended_Wrapped(font, q.q, white, qrect.w);
        if (s) { qtex = SDL_CreateTextureFromSurface(r, s); qrect.h = s->h; SDL_FreeSurface(s); }
        char buf[512];
        snprintf(buf, sizeof(buf), "A. %s", q.a);
        SDL_Surface *sa = TTF_RenderText_Blended_Wrapped(font, buf, white, qrect.w);
        if (sa) { atex = SDL_CreateTextureFromSurface(r, sa); arect.w = sa->w; arect.h = sa->h; SDL_FreeSurface(sa); }
        snprintf(buf, sizeof(buf), "B. %s", q.b);
        SDL_Surface *sb = TTF_RenderText_Blended_Wrapped(font, buf, white, qrect.w);
        if (sb) { btex = SDL_CreateTextureFromSurface(r, sb); brect.w = sb->w; brect.h = sb->h; SDL_FreeSurface(sb); }
        snprintf(buf, sizeof(buf), "C. %s", q.c);
        SDL_Surface *sc = TTF_RenderText_Blended_Wrapped(font, buf, white, qrect.w);
        if (sc) { ctex = SDL_CreateTextureFromSurface(r, sc); crect.w = sc->w; crect.h = sc->h; SDL_FreeSurface(sc); }
    }

    // compute option box sizes and positions, then center them vertically and horizontally
    int opt_pad = 12;
    int box_w = target_w - 80;
    // box heights already stored in arect.h/brect.h/crect.h from text surfaces
    arect.h = arect.h + opt_pad * 2;
    brect.h = brect.h + opt_pad * 2;
    crect.h = crect.h + opt_pad * 2;

    // question rect width is qrect.w; get question text height if available
    int qw = 0, qh = 0;
    if (qtex) SDL_QueryTexture(qtex, NULL, NULL, &qw, &qh);
    int qpanel_h = qh + 12; // padding inside question panel

    int opt_spacing = 12;
    int total_h = qpanel_h + opt_spacing + arect.h + opt_spacing + brect.h + opt_spacing + crect.h;
    int start_y = (target_h - total_h) / 2;
    if (start_y < 10) start_y = 10;

    // question positions
    int qpanel_x = 20;
    int qpanel_w = target_w - 40;
    int qpanel_y = start_y;
    qrect.x = qpanel_x + 6;
    qrect.y = qpanel_y + 6;
    qrect.w = qpanel_w - 12;

    // options centered horizontally
    arect.w = box_w; arect.x = (target_w - box_w) / 2; arect.y = qpanel_y + qpanel_h + opt_spacing;
    brect.w = box_w; brect.x = arect.x; brect.y = arect.y + arect.h + opt_spacing;
    crect.w = box_w; crect.x = arect.x; crect.y = brect.y + brect.h + opt_spacing;

    int running = 1;
    int selected = 0; // 1=A 2=B 3=C
    int show_feedback = 0;
    Uint32 feedback_time = 0;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = 0; break; }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) { running = 0; break; }
                if (!show_feedback) {
                    if (e.key.keysym.sym == SDLK_a) { selected = 1; }
                    else if (e.key.keysym.sym == SDLK_b) { selected = 2; }
                    else if (e.key.keysym.sym == SDLK_c) { selected = 3; }
                    if (selected) {
                        show_feedback = 1;
                        feedback_time = SDL_GetTicks();
                        char selchar = selected==1?'A':(selected==2?'B':'C');
                        fprintf(stderr, "Quiz: selected %c (correct %c)\n", selchar, q.correct);
                        if (app && app->current_player_index >= 0 && app->current_player_index < app->lb.count) {
                            if (selchar == q.correct) {
                                app->lb.scores[app->current_player_index].score += 50;
                                scores_save(app);
                                scores_load(app);
                                fprintf(stderr, "Quiz: correct — added 50 points to %s\n", app->lb.scores[app->current_player_index].player);
                            } else {
                                fprintf(stderr, "Quiz: wrong — no points awarded\n");
                            }
                        }
                    }
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && !show_feedback) {
                int mx = e.button.x, my = e.button.y;
                SDL_Rect r1 = arect, r2 = brect, r3 = crect;
                r1.w = target_w - 60; r2.w = r1.w; r3.w = r1.w;
                if (mx >= r1.x && mx <= r1.x + r1.w && my >= r1.y && my <= r1.y + r1.h) { selected = 1; }
                else if (mx >= r2.x && mx <= r2.x + r2.w && my >= r2.y && my <= r2.y + r2.h) { selected = 2; }
                else if (mx >= r3.x && mx <= r3.x + r3.w && my >= r3.y && my <= r3.y + r3.h) { selected = 3; }
                if (selected) { show_feedback = 1; feedback_time = SDL_GetTicks(); char selchar = selected==1?'A':(selected==2?'B':'C'); fprintf(stderr, "Quiz: selected %c (correct %c)\n", selchar, q.correct); if (app && app->current_player_index >= 0 && app->current_player_index < app->lb.count) { if (selchar == q.correct) { app->lb.scores[app->current_player_index].score += 50; scores_save(app); scores_load(app); fprintf(stderr, "Quiz: correct — added 50 points to %s\n", app->lb.scores[app->current_player_index].player); } else { fprintf(stderr, "Quiz: wrong — no points awarded\n"); } } }
            }
        }

        if (show_feedback) {
            if (SDL_GetTicks() - feedback_time >= 1200) { running = 0; }
        }

        SDL_SetRenderDrawColor(r, 0,0,0,255);
        SDL_RenderClear(r);
        SDL_Rect dst = {0,0,target_w,target_h};
        SDL_RenderCopy(r, bg, NULL, &dst);

        // render question
        if (qtex) {
            int qw = 0, qh = 0; SDL_QueryTexture(qtex, NULL, NULL, &qw, &qh);
            SDL_Rect qr = { (target_w - qw)/2, qrect.y, qw, qh };
            // question background panel
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(r, 0, 0, 0, 160);
            SDL_Rect qpanel = {20, qrect.y - 6, target_w - 40, qr.h + 12};
            SDL_RenderFillRect(r, &qpanel);
            // question text
            SDL_RenderCopy(r, qtex, NULL, &qr);
        }

        // helper to draw option box
        void (*draw_option)(SDL_Texture*, SDL_Rect, int) = NULL;
        draw_option = (void(*)(SDL_Texture*,SDL_Rect,int))0; // placeholder to avoid nested function warnings
        // draw option implementation inline
        {
            // option A
            if (atex) {
                SDL_Rect box = arect;
                SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(r, 20, 20, 20, 200);
                SDL_RenderFillRect(r, &box);
                SDL_SetRenderDrawColor(r, 200, 200, 200, 200);
                SDL_RenderDrawRect(r, &box);
                if (selected == 1 && !show_feedback) { SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND); SDL_SetRenderDrawColor(r, 255,255,255,40); SDL_RenderFillRect(r, &box); }
                int tw=0, th=0; SDL_QueryTexture(atex, NULL, NULL, &tw, &th);
                SDL_Rect tr = { box.x + opt_pad, box.y + (box.h - th)/2, tw, th };
                SDL_RenderCopy(r, atex, NULL, &tr);
                // letter on right
                char lb[2] = {'A','\0'};
                if (font) {
                    SDL_Surface *ls = TTF_RenderText_Blended(font, lb, white);
                    if (ls) {
                        SDL_Texture *lt = SDL_CreateTextureFromSurface(r, ls);
                        int lw = ls->w, lh = ls->h; SDL_FreeSurface(ls);
                        if (lt) {
                            SDL_Rect lrect = { box.x + box.w - lw - opt_pad, box.y + (box.h - lh)/2, lw, lh };
                            SDL_RenderCopy(r, lt, NULL, &lrect);
                            SDL_DestroyTexture(lt);
                        }
                    }
                }
                if (show_feedback && selected == 1) {
                    if ((q.correct == 'A')) SDL_SetRenderDrawColor(r, 0, 180, 0, 200);
                    else SDL_SetRenderDrawColor(r, 200, 0, 0, 200);
                    SDL_RenderDrawRect(r, &box);
                }
            }
            // option B
            if (btex) {
                SDL_Rect box = brect;
                SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(r, 20, 20, 20, 200);
                SDL_RenderFillRect(r, &box);
                SDL_SetRenderDrawColor(r, 200, 200, 200, 200);
                SDL_RenderDrawRect(r, &box);
                if (selected == 2 && !show_feedback) { SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND); SDL_SetRenderDrawColor(r, 255,255,255,40); SDL_RenderFillRect(r, &box); }
                int tw=0, th=0; SDL_QueryTexture(btex, NULL, NULL, &tw, &th);
                SDL_Rect tr = { box.x + opt_pad, box.y + (box.h - th)/2, tw, th };
                SDL_RenderCopy(r, btex, NULL, &tr);
                char lb[2] = {'B','\0'};
                if (font) {
                    SDL_Surface *ls = TTF_RenderText_Blended(font, lb, white);
                    if (ls) {
                        SDL_Texture *lt = SDL_CreateTextureFromSurface(r, ls);
                        int lw = ls->w, lh = ls->h; SDL_FreeSurface(ls);
                        if (lt) {
                            SDL_Rect lrect = { box.x + box.w - lw - opt_pad, box.y + (box.h - lh)/2, lw, lh };
                            SDL_RenderCopy(r, lt, NULL, &lrect);
                            SDL_DestroyTexture(lt);
                        }
                    }
                }
                if (show_feedback && selected == 2) {
                    if ((q.correct == 'B')) SDL_SetRenderDrawColor(r, 0, 180, 0, 200);
                    else SDL_SetRenderDrawColor(r, 200, 0, 0, 200);
                    SDL_RenderDrawRect(r, &box);
                }
            }
            // option C
            if (ctex) {
                SDL_Rect box = crect;
                SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(r, 20, 20, 20, 200);
                SDL_RenderFillRect(r, &box);
                SDL_SetRenderDrawColor(r, 200, 200, 200, 200);
                SDL_RenderDrawRect(r, &box);
                if (selected == 3 && !show_feedback) { SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND); SDL_SetRenderDrawColor(r, 255,255,255,40); SDL_RenderFillRect(r, &box); }
                int tw=0, th=0; SDL_QueryTexture(ctex, NULL, NULL, &tw, &th);
                SDL_Rect tr = { box.x + opt_pad, box.y + (box.h - th)/2, tw, th };
                SDL_RenderCopy(r, ctex, NULL, &tr);
                char lb[2] = {'C','\0'};
                if (font) {
                    SDL_Surface *ls = TTF_RenderText_Blended(font, lb, white);
                    if (ls) {
                        SDL_Texture *lt = SDL_CreateTextureFromSurface(r, ls);
                        int lw = ls->w, lh = ls->h; SDL_FreeSurface(ls);
                        if (lt) {
                            SDL_Rect lrect = { box.x + box.w - lw - opt_pad, box.y + (box.h - lh)/2, lw, lh };
                            SDL_RenderCopy(r, lt, NULL, &lrect);
                            SDL_DestroyTexture(lt);
                        }
                    }
                }
                if (show_feedback && selected == 3) {
                    if ((q.correct == 'C')) SDL_SetRenderDrawColor(r, 0, 180, 0, 200);
                    else SDL_SetRenderDrawColor(r, 200, 0, 0, 200);
                    SDL_RenderDrawRect(r, &box);
                }
            }
        }

        // feedback text
        if (show_feedback) {
            char fb[128];
            char sel = selected==1?'A':(selected==2?'B':'C');
            if (sel == q.correct) snprintf(fb, sizeof(fb), "Correct!");
            else snprintf(fb, sizeof(fb), "Wrong — correct is %c", q.correct);
            if (font) {
                SDL_Surface *sfb = TTF_RenderText_Blended(font, fb, white);
                if (sfb) {
                    SDL_Texture *tfb = SDL_CreateTextureFromSurface(r, sfb);
                    SDL_Rect fr = { (target_w - sfb->w)/2, target_h - sfb->h - 20, sfb->w, sfb->h };
                    SDL_FreeSurface(sfb);
                    if (tfb) { SDL_RenderCopy(r, tfb, NULL, &fr); SDL_DestroyTexture(tfb); }
                }
            }
        }

        SDL_RenderPresent(r);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(bg);
    if (qtex) SDL_DestroyTexture(qtex);
    if (atex) SDL_DestroyTexture(atex);
    if (btex) SDL_DestroyTexture(btex);
    if (ctex) SDL_DestroyTexture(ctex);
    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    if (items) free(items);
}
