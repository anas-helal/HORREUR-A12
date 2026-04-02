#include "ui.h"
#include <stdio.h>
#include <string.h>

int ui_init(App *app) {
    // Initialize button rect
    app->ui.btn.x = 10;
    app->ui.btn.y = 10;
    
    if (app->tex.btn) {
        int bw, bh;
        SDL_QueryTexture(app->tex.btn, NULL, NULL, &bw, &bh);
        int max_btn_w = app->win.win_w / 6;
        float scale = 1.0f;
        if (bw > max_btn_w) scale = (float)max_btn_w / (float)bw;
        app->ui.btn.w = (int)(bw * scale);
        app->ui.btn.h = (int)(bh * scale);
    } else {
        app->ui.btn.w = app->win.win_w / 8;
        app->ui.btn.h = app->win.win_h / 15;
    }

    // Initialize item rects
    int item_w_default = app->win.win_w / 6;
    int item_h_default = app->win.win_h / 15;
    for (int i = 0; i < 3; i++) {
        app->ui.items[i].x = 0;
        app->ui.items[i].y = 0;
        app->ui.items[i].w = item_w_default;
        app->ui.items[i].h = item_h_default;
    }

    // Create text textures for submenu items
    const char *items[] = {"Leaderboard", "Settings", "Exit"};
    if (app->font) {
        SDL_Color text_color = {255, 255, 255, 255};
        for (int i = 0; i < 3; i++) {
            SDL_Surface *text_surf = TTF_RenderText_Solid(app->font, items[i], text_color);
            if (text_surf) {
                app->tex.item_text[i] = SDL_CreateTextureFromSurface(app->win.ren, text_surf);
                SDL_FreeSurface(text_surf);
            }
        }
    }

    return 1;
}

void ui_update_hover(App *app) {
    app->btn_hover = (app->mouse_x >= app->ui.btn.x && app->mouse_x <= app->ui.btn.x + app->ui.btn.w &&
                      app->mouse_y >= app->ui.btn.y && app->mouse_y <= app->ui.btn.y + app->ui.btn.h);
    
    for (int i = 0; i < 3; i++) {
        app->item_hover[i] = (app->mouse_x >= app->ui.items[i].x && app->mouse_x <= app->ui.items[i].x + app->ui.items[i].w &&
                              app->mouse_y >= app->ui.items[i].y && app->mouse_y <= app->ui.items[i].y + app->ui.items[i].h);
    }
}

void ui_render_main_button(App *app) {
    // draw main button (hover if mouse is over it and hover texture exists)
    if (app->btn_hover && app->tex.btn_hover) {
        SDL_RenderCopy(app->win.ren, app->tex.btn_hover, NULL, &app->ui.btn);
    } else if (app->tex.btn) {
        SDL_RenderCopy(app->win.ren, app->tex.btn, NULL, &app->ui.btn);
    } else {
        SDL_SetRenderDrawColor(app->win.ren, 200, 50, 50, 255);
        SDL_RenderFillRect(app->win.ren, &app->ui.btn);
    }

    // render text on main button ("sous menu" or "retour")
    SDL_Texture *btn_text = app->show_sub ? app->tex.btn_text_return : (app->show_leaderboard ? app->tex.btn_text_return : app->tex.btn_text_menu);
    if (btn_text) {
        int tw, th;
        SDL_QueryTexture(btn_text, NULL, NULL, &tw, &th);
        SDL_Rect text_rect = {
            app->ui.btn.x + (app->ui.btn.w - tw) / 2,
            app->ui.btn.y + (app->ui.btn.h - th) / 2,
            tw, th
        };
        SDL_RenderCopy(app->win.ren, btn_text, NULL, &text_rect);
    }
}

void ui_render_submenu(App *app) {
    if (!app->show_sub || !app->tex.sub) return;

    int sw, sh;
    SDL_QueryTexture(app->tex.sub, NULL, NULL, &sw, &sh);
    SDL_Rect dst = {(app->win.win_w - sw) / 2, (app->win.win_h - sh) / 2, sw, sh};
    SDL_RenderCopy(app->win.ren, app->tex.sub, NULL, &dst);

    // draw three submenu item buttons when submenu is shown
    for (int i = 0; i < 3; i++) {
        // show hover texture if mouse is over button, otherwise show normal texture
        if (app->item_hover[i] && app->tex.btn_hover) {
            SDL_RenderCopy(app->win.ren, app->tex.btn_hover, NULL, &app->ui.items[i]);
        } else if (app->tex.item) {
            SDL_RenderCopy(app->win.ren, app->tex.item, NULL, &app->ui.items[i]);
        } else {
            SDL_SetRenderDrawColor(app->win.ren, 100, 100, 100, 255);
            SDL_RenderFillRect(app->win.ren, &app->ui.items[i]);
        }
        
        // render text on the button
        if (app->tex.item_text[i]) {
            int tw, th;
            SDL_QueryTexture(app->tex.item_text[i], NULL, NULL, &tw, &th);
            SDL_Rect text_rect = {
                app->ui.items[i].x + (app->ui.items[i].w - tw) / 2,
                app->ui.items[i].y + (app->ui.items[i].h - th) / 2,
                tw, th
            };
            SDL_RenderCopy(app->win.ren, app->tex.item_text[i], NULL, &text_rect);
        }
    }
}
