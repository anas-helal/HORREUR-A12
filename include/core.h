#ifndef CORE_H
#define CORE_H

#include "app.h"

int core_init(App *app);
void core_cleanup(App *app);
void core_handle_quit(App *app);

#endif
