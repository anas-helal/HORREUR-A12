#ifndef PERSO_H
#define PERSO_H

#include "app.h"

void perso_init(App *app);
int perso_get_run_sprite_index(App *app, int *frame_in_sprite);
void perso_start_attack(App *app);
void perso_update_attack(App *app, unsigned int now_tick);
void perso_handle_event(App *app, const SDL_Event *e);
void perso_update(App *app, float dt, unsigned int now_tick);
void perso_render(App *app);
void perso_spawn(App *app);

#endif