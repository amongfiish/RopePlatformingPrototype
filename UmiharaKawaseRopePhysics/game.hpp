#ifndef game_hpp
#define game_hpp

#include <SDL2/SDL.h>
#ifdef _WIN64
#define main
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
