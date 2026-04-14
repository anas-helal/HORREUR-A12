#include "enemy.h"

#define MOVE_LEFT 1
#define MOVE_RIGHT 2

void initAfficherNPC(NPC *npc, int level)
{
    if (!npc) return;
    memset(npc, 0, sizeof(*npc));
    npc->type = ENEMY;
    npc->level = (level == LEVEL2) ? LEVEL2 : LEVEL1;
    npc->rect = (SDL_Rect){800, (npc->level == LEVEL2) ? 420 : 450, 120, 140};
    npc->health = (npc->level == LEVEL2) ? 75 : 50;
    npc->max_health = npc->health;
    npc->state = ALIVE;
    npc->trajectory = rand() % 2;
    npc->active = 1;
    npc->damage = 10;
}

void deplacementNPC(NPC *npc, int level)
{
    if (!npc || !npc->active) return;
    npc->level = (level == LEVEL2) ? LEVEL2 : LEVEL1;

    {
        int left = (npc->level == LEVEL1) ? 160 : 100;
        int right = (npc->level == LEVEL1) ? 980 : 1080;
        if (npc->rect.x <= left) npc->direction = MOVE_RIGHT;
        if (npc->rect.x >= right) npc->direction = MOVE_LEFT;

        if (npc->trajectory == 0)
        {
            if (rand() % 100 < 8) npc->direction = (rand() % 2) ? MOVE_RIGHT : MOVE_LEFT;
        }
        else
        {
            if (rand() % 100 < 5) npc->direction = (rand() % 2) ? MOVE_RIGHT : MOVE_LEFT;
            if (npc->level == LEVEL2)
            {
                npc->rect.y += (rand() % 3) - 1;
                if (npc->rect.y < 360) npc->rect.y = 360;
                if (npc->rect.y > 520) npc->rect.y = 520;
            }
        }
        if (npc->direction == MOVE_LEFT) npc->rect.x -= 2;
        if (npc->direction == MOVE_RIGHT) npc->rect.x += 2;
    }
}

void animationNPC(NPC *npc, int action)
{
    if (!npc || !npc->active) return;
    npc->direction = action;
    npc->frame = (npc->frame + 1) % 20;
}

int collisionBB(SDL_Rect a, SDL_Rect b)
{
    return !((a.x + a.w) < b.x || a.x > (b.x + b.w) ||
             (a.y + a.h) < b.y || a.y > (b.y + b.h));
}

void gestionSanteNPC(NPC *npc, SDL_Renderer *renderer)
{
    if (!npc || npc->type != ENEMY) return;
    if (npc->health <= 0)
    {
        npc->health = 0;
        npc->state = NEUTRALIZED;
        npc->active = 0;
    }
    else if (npc->health <= npc->max_health / 2)
    {
        npc->state = HURT;
    }
    else
    {
        npc->state = ALIVE;
    }
    if (renderer && npc->active)
    {
        SDL_Rect border = {npc->rect.x + 20, npc->rect.y - 12, 60, 8};
        SDL_Rect fill = {npc->rect.x + 21, npc->rect.y - 11, (npc->health * 58) / npc->max_health, 6};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &border);
        if (npc->state == HURT) SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
        else SDL_SetRenderDrawColor(renderer, 0, 220, 40, 255);
        SDL_RenderFillRect(renderer, &fill);
    }
}
