#include "game.hpp"
#include "player.hpp"
#include "level.hpp"
#include "text.hpp"
using namespace std;

int currentGameState = GAME;
int currentLevelEditorMode = PLATFORM;

Player player;
Level level;

TextBox mousePos;
TextBox platformSize;
TextBox editorMode;

bool gameInit() {
    // top wall
    level.addPlatform(0, -132, 600, 32);
    // left wall
    level.addPlatform(0, -100, 32, 500);
    // right wall
    level.addPlatform(568, -100, 32, 500);
    // bottom left platform
    level.addPlatform(32, 368, 96, 32);
    // bottom right platform
    level.addPlatform(472, 368, 96, 32);
    // middle platform
    level.addPlatform(224, 200, 144, 32);
    // top square
    level.addPlatform(352, 0, 32, 32);
    // top right platform
    level.addPlatform(482, 96, 96, 32);
    // bottom right intermediate platform
    level.addPlatform(536, 268, 32, 32);
    
    level.setStartPos(32, 336);
    
    player.setPos(level.getStartX(), level.getStartY());
    
    return true;
}

void gameCleanUp() {
    player.destroyRope();
}

bool gameUpdate(KeyboardLayout *keys) {
    if (currentGameState == GAME) {
        player.update(keys, &level);
    } else if (currentGameState == LEVEL_EDITOR) {
        
    }
    
    return true;
}

void gameDraw(SDL_Renderer* renderer) {
    level.draw(renderer);
    
    if (currentGameState == GAME) {
        player.draw(renderer);
    } else if (currentGameState == LEVEL_EDITOR) {
        
    }
}
