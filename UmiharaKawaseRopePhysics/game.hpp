#ifndef game_hpp
#define game_hpp

#ifdef __APPLE__
#include <SDL2/SDL.h>
#endif

#ifdef _WIN64
#include <SDL.h>
#endif

#include <string>
#include "controls.hpp"
using namespace std;

enum GameMode {
    GAME,
    LEVEL_EDITOR
};

enum LevelEditorMode {
    PLATFORM,
    START_POINT
};

constexpr int NUMBER_OF_EDITOR_MODES = 2;

const string EDITOR_MODE_STRINGS[2] = {
    "PLATFORM",
    "START POINT"
};


bool gameInit();
void gameCleanUp();

bool gameUpdate(KeyboardLayout *keys);
void gameDraw(SDL_Renderer *renderer);

#endif
