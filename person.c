#include "person.h"

void initPerso(perso *p, SDL_Renderer *renderer)
{
    SDL_Surface *temp = IMG_Load("sprite sheet.png");
    if (!temp) { printf("Erreur sprite: %s\n", IMG_GetError()); return; }
    p->spriteTex = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);

    char filename[20];
    for(int i=0;i<MAX_LIVES;i++){
        sprintf(filename, "hp_%d.png", i);
        temp = IMG_Load(filename);
        if(!temp){ printf("Erreur vie %s: %s\n", filename, IMG_GetError()); continue; }
        p->viesTex[i] = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);
    }

    p->police = TTF_OpenFont("font.ttf", 40);
    if(!p->police){ printf("Erreur font: %s\n", TTF_GetError()); return; }

    p->posPerso = (SDL_Rect){0, SCREEN_H-85, 52, 85};
    p->posVie = (SDL_Rect){10,10,0,0};
    p->posSprite = (SDL_Rect){0,0,52,85};
    p->posScore = (SDL_Rect){10,50,0,0};
    p->score = 0;
    p->frame = 0;
    p->direction = -1;
    p->jump_height=30;
    p->jump_V=0;
    p->up=0;
    p->speed=1;
    p->acceleration=0;
    p->vie=MAX_LIVES-1;
    p->maxVie=MAX_LIVES-1;
    p->lastScoreTime=SDL_GetTicks();
    p->moveDistance=0;
    p->isActive=1;
}

void afficherPerso(perso *p, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, p->spriteTex, &p->posSprite, &p->posPerso);

    if(p->vie>=0 && p->vie<MAX_LIVES) SDL_RenderCopy(renderer, p->viesTex[p->vie], NULL, &p->posVie);

    SDL_Color couleur={0,0,255};
    char s[20]; sprintf(s,"Score: %d",p->score);
    SDL_Surface *scoreSurf=TTF_RenderText_Blended(p->police, s, couleur);
    SDL_Texture *scoreTex=SDL_CreateTextureFromSurface(renderer, scoreSurf);
    SDL_FreeSurface(scoreSurf);
    SDL_RenderCopy(renderer, scoreTex, NULL, &p->posScore);
    SDL_DestroyTexture(scoreTex);

    p->frame++; if(p->frame==7) p->frame=0;
}

void deplacerPerso(perso *p, Uint32 dt)
{
    int oldX=p->posPerso.x;
    switch(p->direction){
        case 1: p->posPerso.x += ((p->acceleration/2)*(dt*dt) + (p->speed*dt)); p->acceleration+=0.02; break;
        case 0: p->posPerso.x -= ((p->acceleration/2)*(dt*dt) + (p->speed*dt)); p->acceleration+=0.02; break;
        default: p->acceleration=0; break;
    }
    if(p->direction!=-1 && p->direction!=-2){
        p->moveDistance += abs(p->posPerso.x-oldX);
        if(p->moveDistance>=50){ p->score+=p->moveDistance/50; p->moveDistance%=50; }
    }
    if(p->up==1){
        p->jump_V+=3; p->posPerso.y+=p->jump_V;
        if(p->posPerso.y>SCREEN_H-85){ p->posPerso.y=SCREEN_H-85; p->jump_V=0; p->up=0; p->score+=5; }
    }
    if(p->posPerso.x<0) p->posPerso.x=0;
    else if(p->posPerso.x>SCREEN_W-p->posPerso.w) p->posPerso.x=SCREEN_W-p->posPerso.w;
}

void animerPerso(perso *p)
{
    int var=p->direction; if(var==-1)var=1; if(var==-2)var=0;
    p->posSprite.x=52*p->frame; p->posSprite.y=85*var; p->posSprite.w=52; p->posSprite.h=85;
}

void updateScore(perso *p, Uint32 currentTime){ if(currentTime-p->lastScoreTime>=1000){ p->score+=1; p->lastScoreTime=currentTime; } }
void updateLifeBar(perso *p){ if(p->vie<0)p->vie=0; else if(p->vie>p->maxVie)p->vie=p->maxVie; }

void decreaseLife(perso *p){ if(p->vie>0) p->vie--; }

void freePerso(perso *p)
{
    if(p->spriteTex) SDL_DestroyTexture(p->spriteTex);
    for(int i=0;i<MAX_LIVES;i++) if(p->viesTex[i]) SDL_DestroyTexture(p->viesTex[i]);
    if(p->police) TTF_CloseFont(p->police);
}

void ajouterPerso(perso *p, SDL_Renderer *renderer)
{
    initPerso(p, renderer);
    p->posPerso.x=200;
    p->posVie.x=300;
    p->posScore.x=300;
    p->isActive=1;
}
