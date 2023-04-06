#include "game.hpp"
#include "player.hpp"
#include "level.hpp"
using namespace std;

Player player;
Platform *level;
int numPlatforms = 0;

bool gameInit() {
    level = new Platform[10];
    numPlatforms = 10;
    
    return true;
}

void gameCleanUp() {
    
}

bool gameUpdate(const Uint8 *keys) {
    player.update(keys, level, numPlatforms);
    
    
    return true;
}

void gameDraw(SDL_Renderer* renderer) {
    
}
