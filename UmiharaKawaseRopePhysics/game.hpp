#ifndef game_hpp
#define game_hpp

#ifdef __APPLE__
#include <SDL2/SDL.h>
#endif

#ifdef _WIN64
#include <SDL.h>
#endif

#include "controls.hpp"

enum GameMode {
    GAME,
    LEVEL_EDITOR
};

enum LevelEditorMode {
    PLATFORM,
    START_POINT,
    END_POINT
};

bool gameInit();
void gameCleanUp();

bool gameUpdate(KeyboardLayout *keys);
void gameDraw(SDL_Renderer *renderer);

#endif
