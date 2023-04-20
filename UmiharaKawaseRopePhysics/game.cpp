#include <filesystem>
#include <vector>

#include "game.hpp"
#include "player.hpp"
#include "level.hpp"
#include "text.hpp"
using namespace std;

const string VERSION = "indev 4";

int currentGameState = MENU;
int currentLevelEditorMode = PLATFORM;
int currentPlatformType = NORMAL;

bool creatingLevel = false;
bool selectingLevel = false;

Player player;
Level level;
string levelFilename;

// menu text
TextBox title;
TextSelection titleOptions;
TextBox levelNameIndicator;
TextInput newLevelName;
TextSelection levelSelector;
vector<filesystem::path> availableLevels;

// pause text
TextBox pauseIndicator;
TextSelection pauseOptions;

// editor text
TextBox editorIndicator;
TextBox editorMode;
TextBox platformType;

int editorCursorX = MAP_WIDTH / 2;
int editorCursorY = MAP_HEIGHT / 2;

void updateLevelEditor(KeyboardLayout *keys);
bool updateMenu(KeyboardLayout *keys, char pressedLetters[], int numPressedLetters);

void updateAvailableLevels();

bool gameInit() {
    // text
    title.initFont();
    editorIndicator.initFont();
    editorMode.initFont();
    platformType.initFont();
    
    title.setText("Grappling Hook Prototype");
    title.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    title.setX(10);
    title.setY(0);
    title.setHeight(64);
    title.detectWidth();
    
    titleOptions.setPos(32, 100);
    titleOptions.setActive(true);
    titleOptions.setScrollable(false);
    titleOptions.addOption("NEW LEVEL", 0xFF, 0xFF, 0xFF, 0xFF);
    titleOptions.addOption("LOAD LEVEL", 0xFF, 0xFF, 0xFF, 0xFF);
    titleOptions.addOption("QUIT", 0xFF, 0xFF, 0xFF, 0xFF);
    
    levelNameIndicator.setText("Level name:");
    levelNameIndicator.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    levelNameIndicator.setX(350);
    levelNameIndicator.setY(100);
    levelNameIndicator.setHeight(32);
    levelNameIndicator.detectWidth();
    
    newLevelName.setX(350);
    newLevelName.setY(150);
    newLevelName.setWidth(200);
    newLevelName.setHeight(52);
    newLevelName.setColor(0x00, 0x00, 0x00, 0xFF);
    
    levelSelector.setPos(350, 250);
    levelSelector.setActive(true);
    levelSelector.setScrollable(true);
    levelSelector.setItemsToDisplay(3);
    
    pauseIndicator.setText("PAUSE");
    pauseIndicator.setX(250);
    pauseIndicator.setY(200);
    pauseIndicator.setHeight(52);
    pauseIndicator.detectWidth();
    pauseIndicator.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    
    pauseOptions.setPos(250, 300);
    pauseOptions.setActive(true);
    pauseOptions.setScrollable(false);
    pauseOptions.addOption("Resume", 0xFF, 0xFF, 0xFF, 0xFF);
    pauseOptions.addOption("Main Menu", 0xFF, 0xFF, 0xFF, 0xFF);
    
    editorIndicator.setText("EDITOR");
    editorIndicator.setColor(0x00, 0xFF, 0x00, 0xFF);
    editorIndicator.setX(10);
    editorIndicator.setY(5);
    editorIndicator.setHeight(30);
    editorIndicator.detectWidth();
    
    editorMode.setColor(0x00, 0xFF, 0x00, 0xFF);
    editorMode.setX(10);
    editorMode.setY(42);
    editorMode.setHeight(30);
    
    platformType.setColor(0x00, 0xFF, 0x00, 0xFF);
    platformType.setX(10);
    platformType.setY(79);
    platformType.setHeight(30);
    
    return true;
}

void gameCleanUp() {
    player.destroyGrappleSeeker();
    player.destroyRope();
}

bool gameUpdate(KeyboardLayout *keys, char pressedLetters[], int numPressedLetters) {
    if (keys->getPlayToggleState() == PRESSED) {
        if (currentGameState == GAME) {
            currentGameState = LEVEL_EDITOR;
        } else if (currentGameState == LEVEL_EDITOR) {
            level.saveLevel(levelFilename);
            
            currentGameState = GAME;
            player.setPos(level.getStartX(), level.getStartY());
            player.destroyRope();
            player.destroyGrappleSeeker();
            player.stop();
        }
    }
    
    if (currentGameState == GAME) {
        if (keys->getPauseState() == PRESSED) {
            currentGameState = PAUSE;
        }
        
        player.update(keys, &level);
    } else if (currentGameState == PAUSE) {
        int pauseSelection = pauseOptions.update(keys);
        
        if (keys->getPauseState() == PRESSED) {
            currentGameState = GAME;
        }
        
        if (pauseSelection == 0) {
            currentGameState = GAME;
        } else if (pauseSelection == 1) {
            level.saveLevel(levelFilename);
            currentGameState = MENU;
        }
    } else if (currentGameState == LEVEL_EDITOR) {
        updateLevelEditor(keys);
    } else if (currentGameState == MENU) {
        if (!updateMenu(keys, pressedLetters, numPressedLetters)) {
            return false;
        }
    }
    
    return true;
}

void gameDraw(SDL_Renderer* renderer) {
    if (currentGameState == GAME || currentGameState == PAUSE) {
        player.draw(renderer);
        level.draw(renderer);
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
    } else if (currentGameState == MENU) {
        title.draw(renderer);
        titleOptions.draw(renderer);
        
        if (creatingLevel) {
            newLevelName.draw(renderer);
            levelNameIndicator.draw(renderer);
        } else if (selectingLevel) {
            levelSelector.draw(renderer);
        }
    }
    
    if (currentGameState == PAUSE) {
        pauseIndicator.draw(renderer);
        pauseOptions.draw(renderer);
    }
}

void updateLevelEditor(KeyboardLayout *keys) {
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
    
    if (keys->getJumpState()) {
        if (currentLevelEditorMode == PLATFORM) {
            int platformExists = level.platformExists(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT);
            
            if (platformExists >= 0 && level.getPlatform(platformExists)->getType() != currentPlatformType) {
                level.removePlatform(platformExists);
            }
            
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
    
    if (keys->getGrappleState()) {
        if (currentLevelEditorMode == PLATFORM) {
            int platform = level.platformExists(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT);
            if (platform >= 0) {
                level.removePlatform(platform);
            }
        }
    }
    
    editorMode.setText("EDITING: " + EDITOR_MODE_STRINGS[currentLevelEditorMode]);
    platformType.setText("PLATFORM TYPE: " + PLATFORM_TYPE_STRINGS[currentPlatformType]);
    editorMode.detectWidth();
    platformType.detectWidth();
}

bool updateMenu(KeyboardLayout *keys, char pressedLetters[], int numPressedLetters) {
    int titleOptionsSelection = titleOptions.update(keys);
    
    if (creatingLevel) {
        if (keys->getConfirmState() == PRESSED) {
            if (newLevelName.getText() != "") {
                levelFilename = newLevelName.getText();
                
                creatingLevel = false;
                titleOptions.setActive(true);
                
                currentGameState = LEVEL_EDITOR;
                
                level.resetLevel();
                level.saveLevel(levelFilename);
                return true;
            }
        } else {
            newLevelName.update(pressedLetters, numPressedLetters);
        }
        
        if (keys->getPauseState() == PRESSED) {
            newLevelName.deleteLast();
        }
        
        if (keys->getBackState() == PRESSED) {
            newLevelName.reset();
            creatingLevel = false;
            titleOptions.setActive(true);
        }
    } else if (selectingLevel) {
        if (availableLevels.size() == 0 || keys->getBackState() == PRESSED) {
            selectingLevel = false;
            titleOptions.setActive(true);
            levelSelector.setActive(false);
            
            levelSelector.clearOptions();
            availableLevels.clear();
        }
        
        if (keys->getConfirmState() == PRESSED) {
            levelFilename = availableLevels[levelSelector.getSelection()];
            level.loadLevel(levelFilename);
            
            currentGameState = GAME;
        }
        
        levelSelector.update(keys);
    }
    
    if (titleOptionsSelection == 0) {
        creatingLevel = true;
        titleOptions.setActive(false);
    } else if (titleOptionsSelection == 1) {
        selectingLevel = true;
        titleOptions.setActive(false);
        levelSelector.setActive(true);
        
        updateAvailableLevels();
    } else if (titleOptionsSelection == 2) {
        return false;
    }
    
    return true;
}

void updateAvailableLevels() {
    filesystem::path levels("levels");
    
    if (!filesystem::exists(levels)) {
        filesystem::create_directory(levels);
    }
    
    for (auto const& dir_entry : std::filesystem::directory_iterator(levels)) {
        if (dir_entry.path().extension() == ".lvl") {
            availableLevels.push_back(dir_entry.path());
            levelSelector.addOption((availableLevels.end()-1)->string(), 0xFF, 0xFF, 0xFF, 0xFF);
        }
    }
}
