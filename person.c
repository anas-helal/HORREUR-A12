#include "person.h"

void initPerso(perso *p, SDL_Renderer *renderer)
{
    p->heartTex = NULL; 
    SDL_Surface *temp = IMG_Load("sprite sheet.png");
    int i;
    char filename[20];
    SDL_Surface *heartSurf;
    if (!temp) return;
    p->spriteTex = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    SDL_SetTextureBlendMode(p->spriteTex, SDL_BLENDMODE_BLEND);

    heartSurf = IMG_Load("pixel-heart.png");
    if(heartSurf){
        p->heartTex = SDL_CreateTextureFromSurface(renderer, heartSurf);
        SDL_FreeSurface(heartSurf);
        SDL_SetTextureBlendMode(p->heartTex, SDL_BLENDMODE_BLEND);
        fprintf(stderr, "DEBUG: pixel-heart.png charge avec succes\n");
    } else {
        fprintf(stderr, "DEBUG ERROR: Impossible de charger pixel-heart.png: %s\n", IMG_GetError());
    }
    fflush(stderr);

    p->vie = MAX_LIVES - 1;
    p->maxVie = MAX_LIVES - 1;

    for(i=0; i<MAX_LIVES; i++){
        p->viesTex[i] = NULL; // Legacy system disabled
    }

    p->police = TTF_OpenFont("font.ttf", 24);
    if(!p->police) return;

    p->posPerso.w = 78;
    p->posPerso.h = 51;
    p->posPerso.x = 100;
    p->posPerso.y = SCREEN_H - 100;
    
    p->posVie.x = 10;
    p->posVie.y = 20;
    p->posVie.w = 220;
    p->posVie.h = 50;

    p->posScore.x = 10;
    p->posScore.y = 70;
    p->posScore.w = 0;
    p->posScore.h = 0;

    p->posSprite.w = 52;
    p->posSprite.h = 34;
    p->posSprite.x = 0;
    p->posSprite.y = 0;

    p->score = 0; p->frame = 0; p->direction = -1;
    p->jump_height = 15; p->jump_V = 0; p->up = 0; p->speed = 5; p->acceleration = 0;
    p->lastScoreTime = SDL_GetTicks(); p->moveDistance = 0; p->isActive = 1; p->isRunning = 0; p->isAttacking = 0;
}

void initialiserJoueur2(perso *p, SDL_Renderer *renderer)
{
    initPerso(p, renderer);
    p->posPerso.x = 700;

    p->posVie.x = SCREEN_W - 210;
    p->posScore.x = SCREEN_W - 210;
}

void afficherPerso(perso *p, SDL_Renderer *renderer)
{
    SDL_Color couleur = {255,255,255};
    char s[20];
    SDL_Surface *scoreSurf;
    SDL_Texture *scoreTex;
    SDL_Rect coeur, score_rect;
    int i, bx, by, viesAffichees;

    // 1. Afficher le perso
    SDL_RenderCopy(renderer, p->spriteTex, &p->posSprite, &p->posPerso);

    // 2. Afficher le score
    sprintf(s,"Score: %d",p->score);
    scoreSurf = TTF_RenderText_Blended(p->police, s, couleur);
    if(scoreSurf){
        score_rect.x = p->posScore.x;
        score_rect.y = p->posScore.y;
        score_rect.w = scoreSurf->w;
        score_rect.h = scoreSurf->h;
        scoreTex = SDL_CreateTextureFromSurface(renderer, scoreSurf);
        SDL_FreeSurface(scoreSurf);
        SDL_RenderCopy(renderer, scoreTex, NULL, &score_rect);
        SDL_DestroyTexture(scoreTex);
    }

    // 3. Afficher les vies
    bx = p->posVie.x;
    by = p->posVie.y;
    viesAffichees = MAX_LIVES;

    if(p->heartTex){
        for(i = 0; i < viesAffichees; i++){
            coeur.x = bx + i * 40;
            coeur.y = by;
            coeur.w = 32;
            coeur.h = 32;
            if(i <= p->vie){
                SDL_SetTextureColorMod(p->heartTex, 255, 255, 255);
            } else {
                SDL_SetTextureColorMod(p->heartTex, 50, 50, 50);
            }
            SDL_RenderCopy(renderer, p->heartTex, NULL, &coeur);
        }
    }

    static int frame_count = 0;
    if(frame_count++ % 100 == 0) {
        fprintf(stderr, "DEBUG: afficherPerso frame=%d, heartTex=%p, vie=%d\n", frame_count, (void*)p->heartTex, p->vie);
        fflush(stderr);
    }

    if(p->direction != -1 || p->isAttacking){
        p->frame++;
        if(p->frame >= 6) p->frame = 0;
    } else {
        p->frame = 0;
    }

    if(p->isAttacking && p->frame == 0) p->isAttacking = 0;
}

void deplacerPerso(perso *p, Uint32 dt)
{
    int horizontalSpeed = p->speed;
    if(p->isRunning) horizontalSpeed = p->speed * 2;
    if(p->direction == 1) p->posPerso.x += horizontalSpeed; 
    else if(p->direction == 0) p->posPerso.x -= horizontalSpeed; 

    if(p->up == 1){
        p->posPerso.y += p->jump_V;
        p->jump_V += 1;
        if(p->posPerso.y >= SCREEN_H - p->posPerso.h){ p->posPerso.y = SCREEN_H - p->posPerso.h; p->jump_V = 0; p->up = 0; }
    }
    if(p->posPerso.x < 0) p->posPerso.x = 0;
    else if(p->posPerso.x > SCREEN_W-p->posPerso.w) p->posPerso.x = SCREEN_W-p->posPerso.w;
}

void animerPerso(perso *p)
{
    int row;
    if(p->isAttacking)
        row = (p->direction == 0) ? 3 : 2;
    else if(p->direction == 1)
        row = 0;
    else if(p->direction == 0)
        row = 1;
    else {
        row = 0; 
        p->frame = 0; 
    }

    p->posSprite.x = 52 * p->frame;
    p->posSprite.y = 34 * row;
    p->posSprite.w = 52;
    p->posSprite.h = 34;
}

void attaquerPerso(perso *p)
{
    if(!p->isAttacking){ p->isAttacking = 1; p->frame = 0; }
}
