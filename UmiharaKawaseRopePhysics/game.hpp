#ifndef game_hpp
#define game_hpp

#include <SDL.h>
#undef main
#include "controls.hpp"

bool gameInit();
void gameCleanUp();

bool gameUpdate(KeyboardLayout *keys);
void gameDraw(SDL_Renderer *renderer);

#endif
