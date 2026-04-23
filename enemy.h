#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#if __has_include(<SDL2/SDL_image.h>)
#include <SDL2/SDL_image.h>
#elif __has_include(<SDL_image.h>)
#include <SDL_image.h>
#endif

#define MAX_ENEMIES 10
#define MAX_POWERUPS 5

// Enum pour le type d'entité
typedef enum {
    ENEMY = 0,
    POWERUP = 1
} EntityType;


typedef enum {
    ALIVE = 0,
    HURT = 1,
    NEUTRALIZED = 2
} HealthState;


typedef enum {
    LEVEL1 = 1,
    LEVEL2 = 2
} GameLevel;


typedef struct 
{
    SDL_Texture *sprite;
    SDL_Rect rect;
    SDL_Rect rects[90];
    int frame;
    int direction;
    int health;
    int max_health;
    HealthState state;
    EntityType type;
    GameLevel level;
    int trajectory;  
    int active;      
    int damage;      
} Entity;
typedef Entity NPC;


typedef struct {
    Entity entities[MAX_ENEMIES + MAX_POWERUPS];
    int count;
    int enemy_count;
    int powerup_count;
    SDL_Texture *enemy_sprite;
    SDL_Texture *powerup_sprite;
} EntityManager;


void initAfficherNPC(NPC *npc, int level);
void deplacementNPC(NPC *npc, int level);
void animationNPC(NPC *npc, int action);
int collisionBB(SDL_Rect a, SDL_Rect b);
void gestionSanteNPC(NPC *npc, SDL_Renderer *renderer);

#endif 
