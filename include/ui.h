#ifndef UI_H
#define UI_H

#include "app.h"

int ui_init(App *app);
void ui_update_hover(App *app);
void ui_render_main_button(App *app);
void ui_render_submenu(App *app);

#endif
