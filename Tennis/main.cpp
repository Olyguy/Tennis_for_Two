#include <SDL.h>
#include <iostream>
#include "Game.h"

Game* game = nullptr;

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    game = new Game();
    game->init("Tennis for Two", 800, 600, false);

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    float delta_time = 0;

    const int FPS = 60; //set fps to 60
    const int FRAME_DELAY = 1000 / FPS;
    Uint32 frame_start;
    Uint32 frame_time;
    while (game->running()) {
        frame_start = SDL_GetTicks();
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        // calculate the difference and convert to seconds (used in power adjustment)
        delta_time = (float)((NOW - LAST) / (float)SDL_GetPerformanceFrequency());

        game->handle_events();
        game->update(delta_time);
        game->render();

        frame_time = SDL_GetTicks() - frame_start;
        if (FRAME_DELAY > frame_time) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }

    game->clean();

    return 0;
}