#include "game.hpp"
#include "player.hpp"
#include "level.hpp"
using namespace std;

Player player;
Platform *level;
int numPlatforms = 0;

bool gameInit() {
    level = new Platform[10];
    numPlatforms = 2;
    
    level[0].setPos(140, 80);
    level[0].setWidth(128);
    level[0].setHeight(48);
    
    level[1].setPos(32, 120);
    level[1].setWidth(32);
    level[1].setHeight(32);
    
    player.setPos(200, 0);
    
    return true;
}

void gameCleanUp() {
    player.destroyRope();
    delete[] level;
}

bool gameUpdate(const Uint8 *keys) {
    player.update(keys, level, numPlatforms);
        
    return true;
}

void gameDraw(SDL_Renderer* renderer) {
    for (int i = 0; i < numPlatforms; i++) {
        level[i].draw(renderer);
    }
    
    player.draw(renderer);
}
