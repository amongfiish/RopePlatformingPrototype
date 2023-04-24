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
    MENU,
    LEVEL_SELECTOR,
    PAUSE,
    GAME,
    LEVEL_EDITOR,
    LEVEL_END,
    LEVEL_RESET
};

enum LevelEditorMode {
    PLATFORM,
    START_POINT,
    END_POINT
};

constexpr int NUMBER_OF_EDITOR_MODES = 3;

const string EDITOR_MODE_STRINGS[3] = {
    "PLATFORM",
    "START POINT",
    "END POINT"
};


bool gameInit();
void gameCleanUp();

bool gameUpdate(KeyboardLayout *keys, char pressedLetters[], int numPressedLetters);
void gameDraw(SDL_Renderer *renderer);

#endif
