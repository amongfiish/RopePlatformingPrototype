#include <cstdio>
#include <SDL2/SDL.h>

#include "game.hpp"
using namespace std;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 400;

bool init();
void cleanUp();

int main(int argc, const char * argv[]) {
    if (!init()) {
        cleanUp();
        return -1;
    }
    
    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }
        
        // update
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        gameUpdate(keys);
        
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        
        // draw
        
        SDL_RenderPresent(renderer);
    }
    
    cleanUp();
    return 0;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL. Error: %s\n", SDL_GetError());
        return false;
    }
    
    window = SDL_CreateWindow("Umihara", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Couldn't create window. Error: %s\n", SDL_GetError());
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Couldn't create renderer. Error: %s\n", SDL_GetError());
        return false;
    }
    
    return true;
}

void cleanUp() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = NULL;
    window = NULL;
    
    SDL_Quit();
}
