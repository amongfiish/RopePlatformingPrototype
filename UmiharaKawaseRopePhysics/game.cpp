#include "game.hpp"
#include "player.hpp"
#include "level.hpp"
#include "text.hpp"
using namespace std;

const string VERSION = "indev 2-3";

int currentGameState = GAME;
int currentLevelEditorMode = PLATFORM;
int currentPlatformType = NORMAL;

Player player;
Level level;

TextBox editorIndicator;
TextBox editorMode;
TextBox platformType;

int editorCursorX = MAP_WIDTH / 2;
int editorCursorY = MAP_HEIGHT / 2;

bool gameInit() {
    level.loadLevel("level");
    
    // text
    editorIndicator.initFont();
    editorMode.initFont();
    platformType.initFont();
    
    editorIndicator.setText("EDITOR");
    editorIndicator.setColor(0x00, 0xFF, 0x00, 0xFF);
    editorIndicator.setX(10);
    editorIndicator.setY(5);
    editorIndicator.setWidth(64);
    editorIndicator.setHeight(32);
    
    editorMode.setColor(0x00, 0xFF, 0x00, 0xFF);
    editorMode.setX(10);
    editorMode.setY(42);
    editorMode.setWidth(128);
    editorMode.setHeight(32);
    
    platformType.setColor(0x00, 0xFF, 0x00, 0xFF);
    platformType.setX(10);
    platformType.setY(79);
    platformType.setWidth(160);
    platformType.setHeight(32);
    
    // player
    player.setPos(level.getStartX(), level.getStartY());
    
    return true;
}

void gameCleanUp() {
    player.destroyGrappleSeeker();
    player.destroyRope();
    
    level.saveLevel("level");
}

bool gameUpdate(KeyboardLayout *keys) {
    if (keys->getPlayToggleState() == PRESSED) {
        if (currentGameState == GAME) {
            currentGameState = LEVEL_EDITOR;
        } else {
            level.saveLevel("level");
            
            currentGameState = GAME;
            player.setPos(level.getStartX(), level.getStartY());
            player.destroyRope();
            player.destroyGrappleSeeker();
            player.stop();
        }
    }
    
    if (currentGameState == GAME) {
        player.update(keys, &level);
    } else if (currentGameState == LEVEL_EDITOR) {
        if (keys->getNextEditorModeState() == PRESSED) {
            currentLevelEditorMode++;
        }
        if (keys->getPreviousEditorModeState() == PRESSED) {
            currentLevelEditorMode--;
        }
        
        if (currentLevelEditorMode >= NUMBER_OF_EDITOR_MODES) {
            currentLevelEditorMode = 0;
        } else if (currentLevelEditorMode < 0) {
            currentLevelEditorMode = NUMBER_OF_EDITOR_MODES - 1;
        }
        
        if (keys->getNextPlatformTypeState() == PRESSED) {
            currentPlatformType++;
        }
        if (keys->getPreviousPlatformTypeState() == PRESSED) {
            currentPlatformType--;
        }
        
        if (currentPlatformType >= NUMBER_OF_PLATFORM_TYPES) {
            currentPlatformType = 0;
        } else if (currentPlatformType < 0) {
            currentPlatformType = NUMBER_OF_PLATFORM_TYPES - 1;
        }
        
        if (keys->getUpState() == PRESSED) {
            editorCursorY -= 1;
        }
        if (keys->getDownState() == PRESSED) {
            editorCursorY += 1;
        }
        
        if (keys->getLeftState() == PRESSED) {
            editorCursorX -= 1;
        }
        if (keys->getRightState() == PRESSED) {
            editorCursorX += 1;
        }
        
        if (editorCursorX < 0) {
            editorCursorX = MAP_WIDTH - 1;
        } else if (editorCursorX >= MAP_WIDTH) {
            editorCursorX = 0;
        }
        
        if (editorCursorY < 0) {
            editorCursorY = MAP_HEIGHT - 1;
        } else if (editorCursorY >= MAP_HEIGHT) {
            editorCursorY = 0;
        }
        
        if (keys->getConfirmState()) {
            if (currentLevelEditorMode == PLATFORM) {
                if (level.platformExists(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT) < 0 &&
                    !(editorCursorX * PLATFORM_WIDTH == level.getStartX() && editorCursorY * PLATFORM_HEIGHT == level.getStartY())) {
                    level.addPlatform(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT, PLATFORM_WIDTH, PLATFORM_HEIGHT, currentPlatformType);
                }
            } else if (currentLevelEditorMode == START_POINT) {
                if (level.platformExists(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT) < 0) {
                    level.setStartPos(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT);
                }
            }
        }
        
        if (keys->getBackState()) {
            if (currentLevelEditorMode == PLATFORM) {
                int platform = level.platformExists(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT);
                if (platform >= 0) {
                    level.removePlatform(platform);
                }
            }
        }
        
        editorMode.setText("EDITING: " + EDITOR_MODE_STRINGS[currentLevelEditorMode]);
        platformType.setText("PLATFORM TYPE: " + PLATFORM_TYPE_STRINGS[currentPlatformType]);
    }
    
    return true;
}

void gameDraw(SDL_Renderer* renderer) {
    level.draw(renderer);
    
    if (currentGameState == GAME) {
        player.draw(renderer);
    } else if (currentGameState == LEVEL_EDITOR) {
        SDL_SetRenderDrawColor(renderer, (currentLevelEditorMode == PLATFORM) ? 0x00 : 0xFF, 0x00, (currentLevelEditorMode == PLATFORM) ? 0xFF : 0x00, 0xFF);
        SDL_Rect cursorRect = { editorCursorX * PLATFORM_WIDTH - 1, editorCursorY * PLATFORM_WIDTH - 1, PLATFORM_WIDTH + 2, PLATFORM_HEIGHT + 2 };
        SDL_RenderDrawRect(renderer, &cursorRect);
        
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_Rect startPosRect = { level.getStartX(), level.getStartY(), PLATFORM_WIDTH, PLATFORM_HEIGHT };
        SDL_RenderFillRect(renderer, &startPosRect);
        
        editorIndicator.draw(renderer);
        editorMode.draw(renderer);
        platformType.draw(renderer);
    }
}
