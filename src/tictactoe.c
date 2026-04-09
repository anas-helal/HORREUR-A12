#include "tictactoe.h"
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include "scores.h"

// helper: check winner for tic-tac-toe (0 none,1 player,2 cpu,3 draw)
static int check_board(const int b[9]) {
    int lines[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    for (int i=0;i<8;i++){
        int a=lines[i][0], c=lines[i][1], d=lines[i][2];
        if (b[a] != 0 && b[a]==b[c] && b[a]==b[d]) return b[a];
    }
    int full=1; for (int i=0;i<9;i++) if (b[i]==0) { full=0; break; }
    if (full) return 3; return 0;
}

// minimax helpers for Tic-Tac-Toe AI
// returns score from CPU (2) perspective: +10 win for CPU, -10 loss, 0 draw
static int minimax(int board[9], int depth, int isCpu) {
    int res = check_board(board);
    if (res != 0) {
        if (res == 2) return 10 - depth;
        if (res == 1) return -10 + depth;
        return 0; // draw
    }

    if (isCpu) {
        int best = -1000;
        for (int i = 0; i < 9; i++) {
            if (board[i] == 0) {
                board[i] = 2;
                int val = minimax(board, depth + 1, 0);
                board[i] = 0;
                if (val > best) best = val;
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 9; i++) {
            if (board[i] == 0) {
                board[i] = 1;
                int val = minimax(board, depth + 1, 1);
                board[i] = 0;
                if (val < best) best = val;
            }
        }
        return best;
    }
}

static int find_best_move(int board[9]) {
    int bestVal = -1000;
    int bestMove = -1;
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) {
            board[i] = 2;
            int moveVal = minimax(board, 0, 0);
            board[i] = 0;
            if (moveVal > bestVal) {
                bestMove = i;
                bestVal = moveVal;
            }
        }
    }
    return bestMove;
}

void tictactoe_run(App *app) {
    const int W = 855, H = 835;
    SDL_Window *w = SDL_CreateWindow("Tic-Tac-Toe",
                                     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     W, H, 0);
    if (!w) return;
    SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!r) { SDL_DestroyWindow(w); return; }

    // try to load a grid image; fall back to drawn grid if unavailable
    SDL_Texture *grid_tex = NULL;
    SDL_Surface *grid_surf = IMG_Load("assets/GrilleTicTacToe.png");
    if (grid_surf) {
        grid_tex = SDL_CreateTextureFromSurface(r, grid_surf);
        SDL_FreeSurface(grid_surf);
    } else {
        fprintf(stderr, "Info: couldn't load assets/GrilleTicTacToe.png: %s\n", IMG_GetError());
    }

    int board[9] = {0}; // 0 empty, 1 player (X), 2 cpu (O)
    int turn = 1; // player starts
    int running = 1;
    int winner = 0; // 0 none, 1 player, 2 cpu, 3 draw

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = 0; break; }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) { running = 0; break; }
                if (e.key.keysym.sym == SDLK_r && winner != 0) {
                    // restart board
                    for (int i=0;i<9;i++) board[i]=0;
                    winner = 0; turn = 1;
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                if (winner != 0) { running = 0; break; }
                if (winner == 0) {
                    int mx = e.button.x, my = e.button.y;
                    int cellw = W/3, cellh = H/3;
                    int cx = mx / cellw; int cy = my / cellh; int idx = cy*3 + cx;
                    if (idx>=0 && idx<9 && board[idx]==0) {
                        board[idx] = 1; // player move
                        winner = check_board(board);
                        if (winner==0) {
                            // cpu move: use improved minimax AI
                            int m = find_best_move(board);
                            if (m >= 0) board[m] = 2;
                            winner = check_board(board);
                        }
                    }
                }
            }
        }

        // render
        SDL_SetRenderDrawColor(r, 30, 30, 30, 255);
        SDL_RenderClear(r);
        if (grid_tex) {
            SDL_RenderCopy(r, grid_tex, NULL, NULL);
        } else {
            SDL_SetRenderDrawColor(r, 200,200,200,255);
            // grid lines
            for (int i=1;i<=2;i++) {
                SDL_RenderDrawLine(r, i*(W/3), 0, i*(W/3), H);
                SDL_RenderDrawLine(r, 0, i*(H/3), W, i*(H/3));
            }
        }
        // draw X/O
        for (int i=0;i<9;i++){
            int v = board[i];
            int cx = (i%3)*(W/3);
            int cy = (i/3)*(H/3);
            SDL_Rect cell = {cx+10, cy+10, W/3-20, H/3-20};
            if (v==1) {
                // X
                SDL_SetRenderDrawColor(r, 220,40,40,255);
                SDL_RenderDrawLine(r, cell.x, cell.y, cell.x+cell.w, cell.y+cell.h);
                SDL_RenderDrawLine(r, cell.x+cell.w, cell.y, cell.x, cell.y+cell.h);
            } else if (v==2) {
                // O (rectangle ring)
                SDL_SetRenderDrawColor(r, 40,120,220,255);
                SDL_Rect outer = cell;
                SDL_Rect inner = {cell.x+6, cell.y+6, cell.w-12, cell.h-12};
                SDL_RenderDrawRect(r, &outer);
                SDL_SetRenderDrawColor(r, 30,30,30,255);
                SDL_RenderDrawRect(r, &inner);
            }
        }

        // if winner, show overlay with message and controls (restart/close)
        if (winner!=0) {
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(r, 0,0,0,200);
            SDL_Rect o = {0,0,W,H}; SDL_RenderFillRect(r,&o);

            const char *msg = "";
            if (winner == 1) msg = "You win!";
            else if (winner == 2) msg = "CPU wins";
            else msg = "Draw";

            if (app->font) {
                SDL_Color white = {255,255,255,255};
                char buf[128];
                snprintf(buf, sizeof(buf), "%s  (R to restart, click/ESC to close)", msg);
                SDL_Surface *surf = TTF_RenderText_Solid(app->font, buf, white);
                if (surf) {
                    SDL_Texture *txt = SDL_CreateTextureFromSurface(r, surf);
                    int tw = surf->w, th = surf->h;
                    SDL_FreeSurface(surf);
                    if (txt) {
                        SDL_Rect dst = {(W - tw) / 2, (H - th) / 2, tw, th};
                        SDL_RenderCopy(r, txt, NULL, &dst);
                        SDL_DestroyTexture(txt);
                    }
                }
            }
        }

        SDL_RenderPresent(r);
        SDL_Delay(16);
    }

    // Update player score according to outcome: win +100, draw +50, loss +0
    if (app && app->current_player_index >= 0 && app->current_player_index < app->lb.count) {
        if (winner == 1) {
            app->lb.scores[app->current_player_index].score += 100;
        } else if (winner == 3) {
            app->lb.scores[app->current_player_index].score += 50;
        } else {
            // loss -> no change
        }
        // persist and reload leaderboard textures
        scores_save(app);
        scores_load(app);
    }

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    if (grid_tex) SDL_DestroyTexture(grid_tex);
}
