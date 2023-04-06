#ifndef game_hpp
#define game_hpp

#include <SDL2/SDL.h>

bool gameInit();
void gameCleanUp();

bool gameUpdate(const Uint8 *keys);
void gameDraw(SDL_Renderer *renderer);

#endif
