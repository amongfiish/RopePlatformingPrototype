#include "game.hpp"
#include "player.hpp"
#include "level.hpp"
using namespace std;

Player player;
Platform *level;
int numPlatforms = 0;

bool gameInit() {
    level = new Platform[10];
    numPlatforms = 1;
    
    level[0].setPos(140, 100);
    level[0].setWidth(32);
    level[0].setHeight(32);
    
    player.setPos(0, 95);
    player.createRope(100, 0);
    player.setPos(0, 100);
    
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
