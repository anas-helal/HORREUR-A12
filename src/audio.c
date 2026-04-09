#include "audio.h"
#include <stdio.h>

int audio_init(App *app) {
    app->audio.bgm = Mix_LoadMUS("assets/horror_sfx.mp3");
    if (!app->audio.bgm) {
        fprintf(stderr, "Warning: couldn't load horror_sfx.mp3: %s\n", Mix_GetError());
        return 0;
    }
    
    Mix_PlayMusic(app->audio.bgm, -1);
    return 1;
}

void audio_cleanup(App *app) {
    if (app->audio.bgm) Mix_FreeMusic(app->audio.bgm);
}
