#include "enemy.h"

#define HERO_START_X 80
#define HERO_START_Y 500
#define ENNEMI_FRAME_W 390
#define ENNEMI_FRAME_H 432

static void remplir_niveau(EntityManager *manager, int level, SDL_Texture *enemy_tex)
{
    manager->count = 0;
    manager->enemy_count = 0;
    manager->powerup_count = 0;

    int enemy_total = (level == LEVEL1) ? 3 : 4;
    SDL_Point pos_lvl1[] = {{760, 430}, {980, 470}, {860, 450}};
    SDL_Point pos_lvl2[] = {{700, 400}, {900, 450}, {1050, 420}, {830, 500}};

    for (int i = 0; i < enemy_total; i++)
    {
        NPC *e = &manager->entities[manager->count++];
        initAfficherNPC(e, level);
        if (level == LEVEL1)
        {
            e->rect.x = pos_lvl1[i].x;
            e->rect.y = pos_lvl1[i].y;
        }
        else
        {
            e->rect.x = pos_lvl2[i].x;
            e->rect.y = pos_lvl2[i].y;
        }
        e->sprite = enemy_tex;
        manager->enemy_count++;
    }

    NPC *p1 = &manager->entities[manager->count++];
    memset(p1, 0, sizeof(*p1));
    p1->type = POWERUP;
    p1->rect = (SDL_Rect){520, 430, 30, 30};
    p1->active = 1;
    p1->damage = 1;

    NPC *p2 = &manager->entities[manager->count++];
    memset(p2, 0, sizeof(*p2));
    p2->type = POWERUP;
    p2->rect = (SDL_Rect){620, 480, 30, 30};
    p2->active = 1;
    p2->damage = 2;
    manager->powerup_count = 2;
}

static void process_collisions(EntityManager *manager, SDL_Rect hero_rect, EntityType type,
    int *player_health, int *score, Uint32 *last_hit_time, Uint32 cooldown_ms)
{
    for (int i = 0; i < manager->count; i++)
    {
        Entity *e = &manager->entities[i];
        if (!e->active || e->type != type) continue;

        if (!collisionBB(hero_rect, e->rect)) continue;

        if (type == ENEMY)
        {
            Uint32 now = SDL_GetTicks();
            if (now - *last_hit_time >= cooldown_ms)
            {
                *player_health -= e->damage;
                e->health -= 15;
                gestionSanteNPC(e, NULL);
                *last_hit_time = now;
            }
        }
        else
        {
            if (e->damage == 1)
            {
                *player_health += 20;
                if (*player_health > 100) *player_health = 100;
            }
            else if (e->damage == 2)
            {
                *score += 50;
            }
            e->active = 0;
        }
    }
}

static void mettre_a_jour_et_animer(EntityManager *manager, int level, SDL_Rect hero_rect)
{
    for (int i = 0; i < manager->count; i++)
    {
        NPC *e = &manager->entities[i];
        if (!e->active || e->type != ENEMY) continue;

        /* Loin du joueur: deplacement aleatoire, proche: animation d'attaque */
        int distance_x = hero_rect.x - e->rect.x;
        if (distance_x < -140 || distance_x > 140)
        {
            deplacementNPC(e, level);
            animationNPC(e, e->direction);
        }
        else
        {
            if (distance_x < 0) animationNPC(e, 3); /* ATTACK_LEFT */
            else animationNPC(e, 4);                /* ATTACK_RIGHT */
        }
    }
}

static void afficher_entites(EntityManager *manager, SDL_Renderer *renderer)
{
    for (int i = 0; i < manager->count; i++)
    {
        NPC *e = &manager->entities[i];
        if (!e->active) continue;

        if (e->type == POWERUP)
        {
            continue;
        }

        if (e->sprite)
        {
            int row = 0;
            int col = e->frame % 10;
            if (e->direction == 2) row = 1;
            else if (e->direction == 3) row = 2;
            else if (e->direction == 4) row = 3;
            SDL_Rect src = {col * ENNEMI_FRAME_W, row * ENNEMI_FRAME_H, ENNEMI_FRAME_W, ENNEMI_FRAME_H};
            SDL_RenderCopy(renderer, e->sprite, &src, &e->rect);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 220, 60, 60, 255);
            SDL_RenderFillRect(renderer, &e->rect);
        }

        gestionSanteNPC(e, renderer);
    }
}

int main(void)
{
    const int SCREEN_WIDTH = 1200;
    const int SCREEN_HEIGHT = 720;
    srand((unsigned int)time(NULL));

    bool running = true;
    SDL_Event event;
    int current_level = LEVEL1;
    int player_health = 100;
    Uint32 last_hit_time = 0;
    const Uint32 hit_cooldown_ms = 400;
    int score = 0;
    int image_ok = 0;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;
#if __has_include(<SDL2/SDL_image.h>) || __has_include(<SDL_image.h>)
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG)) image_ok = 1;
#endif

    SDL_Window *window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return 1;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) return 1;

    SDL_Texture *bg = NULL;
    SDL_Texture *hero = NULL;
    SDL_Texture *enemy_tex = NULL;
    if (image_ok)
    {
        bg = IMG_LoadTexture(renderer, "Assets/graphic/stages/Background.jpg");
        hero = IMG_LoadTexture(renderer, "Assets/graphic/hero/hero_test.png");
        enemy_tex = IMG_LoadTexture(renderer, "Assets/graphic/enemy/ennemie.png");
        if (!enemy_tex) enemy_tex = IMG_LoadTexture(renderer, "Assets/graphic/enemy/enemy.png");
    }

    SDL_Rect hero_rect = {HERO_START_X, HERO_START_Y, 120, 150};

    EntityManager manager;
    memset(&manager, 0, sizeof(manager));
    remplir_niveau(&manager, LEVEL1, enemy_tex);

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_RIGHT:
                    hero_rect.x += 20;
                    if (hero_rect.x > SCREEN_WIDTH - hero_rect.w) hero_rect.x = SCREEN_WIDTH - hero_rect.w;
                    break;
                case SDLK_LEFT:
                    hero_rect.x -= 20;
                    if (hero_rect.x < 0) hero_rect.x = 0;
                    break;
                case SDLK_UP:
                    hero_rect.y -= 20;
                    if (hero_rect.y < 0) hero_rect.y = 0;
                    break;
                case SDLK_DOWN:
                    hero_rect.y += 20;
                    if (hero_rect.y > SCREEN_HEIGHT - hero_rect.h) hero_rect.y = SCREEN_HEIGHT - hero_rect.h;
                    break;
                case SDLK_l:
                    current_level = (current_level == LEVEL1) ? LEVEL2 : LEVEL1;
                    remplir_niveau(&manager, current_level, enemy_tex);
                    hero_rect.x = HERO_START_X;
                    hero_rect.y = HERO_START_Y;
                    break;
                }
            }
        }

        mettre_a_jour_et_animer(&manager, current_level, hero_rect);
        process_collisions(&manager, hero_rect, ENEMY, &player_health, &score, &last_hit_time, hit_cooldown_ms);
        process_collisions(&manager, hero_rect, POWERUP, &player_health, &score, &last_hit_time, hit_cooldown_ms);

        if (player_health <= 0)
        {
            player_health = 100;
            hero_rect.x = HERO_START_X;
            hero_rect.y = HERO_START_Y;
        }

        SDL_SetRenderDrawColor(renderer, 18, 18, 35, 255);
        SDL_RenderClear(renderer);
        if (bg)
        {
            SDL_Rect bg_dest = {-40, 0, SCREEN_WIDTH + 80, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, bg, NULL, &bg_dest);
 
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 40, 180, 90, 255);
            SDL_RenderFillRect(renderer, &(SDL_Rect){0, SCREEN_HEIGHT - 120, SCREEN_WIDTH, 120});
        }

        if (hero) SDL_RenderCopy(renderer, hero, NULL, &hero_rect);
        else
        {
            SDL_SetRenderDrawColor(renderer, 60, 150, 240, 255);
            SDL_RenderFillRect(renderer, &hero_rect);
        }
        afficher_entites(&manager, renderer);

        SDL_Rect hp = {20, 20, (player_health * 2), 18};
        SDL_SetRenderDrawColor(renderer, 180, 0, 0, 255);
        SDL_RenderFillRect(renderer, &hp);

        SDL_RenderPresent(renderer);
    }

    (void)manager;
    if (enemy_tex) SDL_DestroyTexture(enemy_tex);
    if (hero) SDL_DestroyTexture(hero);
    if (bg) SDL_DestroyTexture(bg);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
#if __has_include(<SDL2/SDL_image.h>) || __has_include(<SDL_image.h>)
    if (image_ok) IMG_Quit();
#endif
    SDL_Quit();
    return 0;
}
