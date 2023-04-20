#include <cstdio>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif

#ifdef _WIN64
#include <SDL.h>
#include <SDL_ttf.h>
#endif

#include "game.hpp"
#include "level.hpp"
#include "controls.hpp"
using namespace std;

KeyboardLayout defaultLayout;
KeyboardLayout *activeKeyboardLayout = NULL;
MouseState mouseState;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

const int WINDOW_WIDTH = MAP_WIDTH * PLATFORM_WIDTH;
const int WINDOW_HEIGHT = MAP_HEIGHT * PLATFORM_HEIGHT;

const int FPS = 60;

char pressedLetters[50];
int numPressedLetters = 0;

bool init();
void cleanUp();

int main(int argc, char* argv[]) {
    if (!init() || !gameInit()) {
        cleanUp();
        return -1;
    }
    
    SDL_Event e;
    bool running = true;
    while (running) {
        Uint32 startTicks = SDL_GetTicks();
        
        numPressedLetters = 0;
        
        mouseState.update();
        
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                string keyName = SDL_GetKeyName(e.key.keysym.sym);
                if (keyName.length() == 1 && ((keyName[0] >= 'A' && keyName[0] <= 'Z') || (keyName[0] >= '0' && keyName[0] <= '9'))) {
                    pressedLetters[numPressedLetters] = keyName[0];
                    numPressedLetters++;
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouseState.setLeftClickState(PRESSED);
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    mouseState.setRightClickState(PRESSED);
                } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                    mouseState.setMiddleClickState(PRESSED);
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouseState.setLeftClickState(NONE);
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    mouseState.setRightClickState(NONE);
                } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                    mouseState.setMiddleClickState(NONE);
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                mouseState.setMovementX(e.motion.xrel);
                mouseState.setMovementY(e.motion.yrel);
            } else if (e.type == SDL_MOUSEWHEEL) {
                mouseState.setScrollX(e.wheel.x);
                mouseState.setScrollY(e.wheel.y);
            }
        }
        
        // update
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        activeKeyboardLayout->update(keys);
        
        if (!gameUpdate(activeKeyboardLayout, pressedLetters, numPressedLetters)) {
            running = false;
        }
        
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        
        // draw
        gameDraw(renderer);
        
        SDL_RenderPresent(renderer);
        
        Uint32 elapsedTicks = SDL_GetTicks() - startTicks;
        if (elapsedTicks < 1000.0 / FPS) {
            SDL_Delay(static_cast<Uint32>(1000.0 / FPS - elapsedTicks));
        }
    }
    
    cleanUp();
    return 0;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL. Error: %s\n", SDL_GetError());
        return false;
    }
    
    if (TTF_Init() < 0) {
        printf("Couldn't initialize TTF. Error: %s\n", SDL_GetError());
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
    
    defaultLayout.setUp(SDL_SCANCODE_UP);
    defaultLayout.setDown(SDL_SCANCODE_DOWN);
    defaultLayout.setLeft(SDL_SCANCODE_LEFT);
    defaultLayout.setRight(SDL_SCANCODE_RIGHT);
    
    defaultLayout.setConfirm(SDL_SCANCODE_RETURN);
    defaultLayout.setBack(SDL_SCANCODE_ESCAPE);
    defaultLayout.setPause(SDL_SCANCODE_BACKSPACE);
    
    defaultLayout.setJump(SDL_SCANCODE_C);
    defaultLayout.setGrapple(SDL_SCANCODE_X);
    defaultLayout.setAirBlast(-1);
    
    defaultLayout.setNextEditorMode(SDL_SCANCODE_E);
    defaultLayout.setPreviousEditorMode(SDL_SCANCODE_Q);
    defaultLayout.setNextPlatformType(SDL_SCANCODE_A);
    defaultLayout.setPreviousPlatformType(SDL_SCANCODE_D);
    defaultLayout.setPlayToggle(SDL_SCANCODE_SPACE);
    
    activeKeyboardLayout = &defaultLayout;
    
    return true;
}

void cleanUp() {
    gameCleanUp();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = NULL;
    window = NULL;
    
    SDL_Quit();
}
