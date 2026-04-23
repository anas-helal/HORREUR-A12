#include "background.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

void loadBackground1(Background *b, SDL_Renderer *renderer, char *path)
{
    SDL_Surface *surf = IMG_Load(path);
    b->img1 = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_QueryTexture(b->img1, NULL, NULL, &b->width1, &b->height1);
    SDL_FreeSurface(surf);
}

void loadBackground2(Background *b, SDL_Renderer *renderer, char *path)
{
    SDL_Surface *surf = IMG_Load(path);
    b->img2 = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_QueryTexture(b->img2, NULL, NULL, &b->width2, &b->height2);
    SDL_FreeSurface(surf);
}

void initBackground(Background *b, SDL_Renderer *renderer, char *path)
{
    loadBackground1(b, renderer, path);
    loadBackground2(b, renderer, path);

    b->camera1 = (SDL_Rect){0, 0, 640, 720};
    b->camera2 = (SDL_Rect){0, 0, 640, 720};

    b->posEcran1 = (SDL_Rect){0, 0, 640, 720};
    b->posEcran2 = (SDL_Rect){640, 0, 640, 720};
}

void afficherBackground(Background b, SDL_Renderer *renderer, int mode)
{
    if(mode == 0)
    {
        SDL_RenderCopy(renderer, b.img1, &b.camera1, NULL);
    }
    else
    {
        SDL_RenderCopy(renderer, b.img1, &b.camera1, &b.posEcran1);
        SDL_RenderCopy(renderer, b.img2, &b.camera2, &b.posEcran2);
    }
}

void scrolling(Background *b, int dx, int dy, int numBack)
{
    SDL_Rect *cam;
    int width, height;

    if(numBack == 1)
    {
        cam = &b->camera1;
        width = b->width1;
        height = b->height1;
    }
    else
    {
        cam = &b->camera2;
        width = b->width2;
        height = b->height2;
    }

    cam->x += dx;
    cam->y += dy;

    if(cam->x < 0) cam->x = 0;
    if(cam->y < 0) cam->y = 0;

    if(cam->x > width - cam->w)
        cam->x = width - cam->w;

    if(cam->y > height - cam->h)
        cam->y = height - cam->h;
}

void afficherInterface(SDL_Renderer *renderer, int score, int vie, int temps, TTF_Font *font)
{
    SDL_Color color = {255,255,255};

    char texte[100];
    sprintf(texte, "Score:%d Vie:%d Temps:%d", score, vie, temps);

    SDL_Surface *surf = TTF_RenderText_Blended(font, texte, color);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_Rect pos = {20, 20, surf->w, surf->h};

    SDL_FreeSurface(surf);
    SDL_RenderCopy(renderer, tex, NULL, &pos);
    SDL_DestroyTexture(tex);
}
