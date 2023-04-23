#include <filesystem>
#include <vector>

#include "game.hpp"
#include "player.hpp"
#include "level.hpp"
#include "text.hpp"
using namespace std;

const string VERSION = "indev 6";

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
TextBox versionIndicator;

// level end text
TextBox winIndicator;
TextBox timeIndicator;
TextBox fastestIndicator;
TextSelection endOptions;

// pause text
TextBox pauseIndicator;
TextSelection pauseOptions;

// editor text
TextBox editorIndicator;
TextBox editorMode;
TextBox platformType;

Uint64 startTicks;
double secondsTaken;

// in game text
TextBox timer;

int editorCursorX = MAP_WIDTH / 2;
int editorCursorY = MAP_HEIGHT / 2;

void updateLevelEditor(KeyboardLayout *keys);
bool updateMenu(KeyboardLayout *keys, char pressedLetters[], int numPressedLetters);
void resetLevel();

void updateAvailableLevels();

bool gameInit() {
    filesystem::path levels("levels");
    
    if (!filesystem::exists(levels)) {
        filesystem::create_directory(levels);
    }
    
    // menu text
    title.initFont();
    editorIndicator.initFont();
    versionIndicator.initFont();
    editorMode.initFont();
    platformType.initFont();
    winIndicator.initFont();
    timeIndicator.initFont();
    
    title.setText("Grappling Hook Prototype");
    title.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    title.setX(10);
    title.setY(0);
    title.setHeight(64);
    title.detectWidth();
    
    titleOptions.setPos(32, 100);
    titleOptions.setActive(true);
    titleOptions.setScrollable(false);
    titleOptions.addOption("CAMPAIGN", 0xFF, 0xFF, 0xFF, 0xFF);
    titleOptions.addOption("NEW LEVEL", 0xFF, 0xFF, 0xFF, 0xFF);
    titleOptions.addOption("LOAD LEVEL", 0xFF, 0xFF, 0xFF, 0xFF);
    titleOptions.addOption("QUIT", 0xFF, 0xFF, 0xFF, 0xFF);
    
    versionIndicator.setText(VERSION);
    versionIndicator.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    versionIndicator.setX(675);
    versionIndicator.setY(450);
    versionIndicator.setHeight(24);
    versionIndicator.detectWidth();
    
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
    
    // level end text
    winIndicator.setText("Level End");
    winIndicator.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    winIndicator.setX(250);
    winIndicator.setY(100);
    winIndicator.setHeight(64);
    winIndicator.detectWidth();
    
    timeIndicator.setColor(0xFF, 0xFF, 0x00, 0xFF);
    timeIndicator.setX(250);
    timeIndicator.setY(170);
    timeIndicator.setHeight(32);
    timeIndicator.detectWidth();
    
    fastestIndicator.setColor(0xFF, 0xFF, 0x00, 0xFF);
    fastestIndicator.setX(250);
    fastestIndicator.setY(210);
    fastestIndicator.setHeight(32);
    fastestIndicator.detectWidth();
    
    endOptions.setPos(250, 250);
    endOptions.setActive(true);
    endOptions.setScrollable(false);
    endOptions.addOption("RETRY", 0xFF, 0xFF, 0xFF, 0xFF);
    endOptions.addOption("EDIT", 0xFF, 0xFF, 0xFF, 0xFF);
    endOptions.addOption("RESET FASTEST", 0xFF, 0xFF, 0xFF, 0xFF);
    endOptions.addOption("MAIN MENU", 0xFF, 0xFF, 0xFF, 0xFF);
    
    // pause text
    pauseIndicator.setText("IN-GAME MENU");
    pauseIndicator.setX(150);
    pauseIndicator.setY(100);
    pauseIndicator.setHeight(52);
    pauseIndicator.detectWidth();
    pauseIndicator.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    
    pauseOptions.setPos(150, 200);
    pauseOptions.setActive(true);
    pauseOptions.setScrollable(false);
    pauseOptions.addOption("Resume", 0xFF, 0xFF, 0xFF, 0xFF);
    pauseOptions.addOption("Main Menu", 0xFF, 0xFF, 0xFF, 0xFF);
    
    // editor text
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
    
    // game text
    timer.setColor(0x88, 0x88, 0x00, 0xFF);
    timer.setX(10);
    timer.setY(5);
    timer.setHeight(24);
    
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
            level.setFastestTime(-1);
            level.saveLevel(levelFilename);
            
            currentGameState = GAME;
            
            resetLevel();
        }
    }
    
    if (currentGameState == GAME) {
        char timerText[50];
        snprintf(timerText, 50, "%.3f", (SDL_GetTicks64() - startTicks) / 1000.0);
        timer.setText(timerText);
        timer.detectWidth();
        
        if (keys->getPauseState() == PRESSED) {
            currentGameState = PAUSE;
        }
        
        if (keys->getResetState() == PRESSED) {
            resetLevel();
        }
        
        if (level.collideEndX(player.getX(), player.getY(), player.getWidth(), player.getHeight())) {
            currentGameState = LEVEL_END;
            secondsTaken = (SDL_GetTicks64() - startTicks) / 1000.0;
            
            bool newFastest = false;
            if (secondsTaken < level.getFastestTime() || level.getFastestTime() < 0) {
                level.setFastestTime(secondsTaken);
                level.saveLevel(levelFilename);
                newFastest = true;
            }
            
            char s[50];
            snprintf(s, 50, "Time: %.3f secs", secondsTaken);
            
            timeIndicator.setText(s);
            timeIndicator.detectWidth();
            
            if (!newFastest) {
                snprintf(s, 50, "Fastest: %.3f secs", level.getFastestTime());
            } else {
                snprintf(s, 50, "Fastest: %.3f secs *NEW*", level.getFastestTime());
            }
                
            fastestIndicator.setText(s);
            fastestIndicator.detectWidth();
        }
        
        if (!player.update(keys, &level)) {
            resetLevel();
        }
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
            level.resetLevel();
            
            resetLevel();
        }
    } else if (currentGameState == LEVEL_EDITOR) {
        updateLevelEditor(keys);
    } else if (currentGameState == MENU) {
        if (!updateMenu(keys, pressedLetters, numPressedLetters)) {
            return false;
        }
    } else if (currentGameState == LEVEL_END) {
        int endOption = endOptions.update(keys);
        
        if (endOption == 0) {
            resetLevel();
            currentGameState = GAME;
        } else if (endOption == 1) {
            resetLevel();
            currentGameState = LEVEL_EDITOR;
        } else if (endOption == 2) {
            level.setFastestTime(-1);
            level.saveLevel(levelFilename);
            fastestIndicator.setText("Fastest: no data");
            fastestIndicator.detectWidth();
        } else if (endOption == 3) {
            level.saveLevel(levelFilename);
            level.resetLevel();
            currentGameState = MENU;
        }
    }
    
    return true;
}

void gameDraw(SDL_Renderer* renderer) {
    if (currentGameState == GAME) {
        player.draw(renderer);
        level.draw(renderer);
        timer.draw(renderer);
    } else if (currentGameState == PAUSE) {
        pauseIndicator.draw(renderer);
        pauseOptions.draw(renderer);
    } else if (currentGameState == LEVEL_EDITOR) {
        level.draw(renderer);
        
        int r, g, b, a;
        if (currentLevelEditorMode == PLATFORM) {
            r = 0x00;
            g = 0xFF;
            b = 0x00;
            a = 0xFF;
        } else if (currentLevelEditorMode == START_POINT) {
            r = 0xFF;
            g = 0x00;
            b = 0x00;
            a = 0xFF;
        } else if (currentLevelEditorMode == END_POINT) {
            r = 0xFF;
            g = 0xFF;
            b = 0x00;
            a = 0xFF;
        } else {
            r = 0x00;
            g = 0x00;
            b = 0x00;
            a = 0x00;
        }
        
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
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
        versionIndicator.draw(renderer);
        
        if (creatingLevel) {
            newLevelName.draw(renderer);
            levelNameIndicator.draw(renderer);
        } else if (selectingLevel) {
            levelSelector.draw(renderer);
        }
    } else if (currentGameState == LEVEL_END) {
        winIndicator.draw(renderer);
        timeIndicator.draw(renderer);
        fastestIndicator.draw(renderer);
        endOptions.draw(renderer);
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
    
    int platformExists = level.platformExists(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT);
    int startPosHere = (editorCursorX * PLATFORM_WIDTH == level.getStartX() && editorCursorY * PLATFORM_HEIGHT == level.getStartY());
    int endPosHere = (editorCursorX * PLATFORM_WIDTH == level.getEndX() && editorCursorY * PLATFORM_HEIGHT == level.getEndY());
    
    if (keys->getJumpState()) {
        if (currentLevelEditorMode == PLATFORM) {
            if (platformExists >= 0 &&
                level.getPlatform(platformExists)->getType() != currentPlatformType) {
                
                level.removePlatform(platformExists);
                level.addPlatform(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT, PLATFORM_WIDTH, PLATFORM_HEIGHT, currentPlatformType);
            } else if (platformExists < 0 &&
                !startPosHere &&
                !endPosHere) {
                
                level.addPlatform(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT, PLATFORM_WIDTH, PLATFORM_HEIGHT, currentPlatformType);
            }
        } else if (currentLevelEditorMode == START_POINT) {
            if (platformExists < 0 &&
                !endPosHere) {
                
                level.setStartPos(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT);
            }
        } else if (currentLevelEditorMode == END_POINT) {
            if (platformExists < 0 &&
                !startPosHere) {
                
                level.setEndPos(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT);
            }
        }
    }
    
    if (keys->getGrappleState()) {
        if (currentLevelEditorMode == PLATFORM) {
            if (platformExists >= 0) {
                level.removePlatform(platformExists);
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
                
                player.setPos(level.getStartX(), level.getStartY());
                
                level.setStartPos(0, 0);
                level.setEndPos(64, 64);
                level.setFastestTime(-1);
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
            levelFilename = availableLevels[levelSelector.getSelection()].string();
            level.loadLevel(levelFilename);
            
            player.setPos(level.getStartX(), level.getStartY());
            
            currentGameState = GAME;
            
            resetLevel();
        }
        
        levelSelector.update(keys);
    }
    
    if (titleOptionsSelection == 1) {
        creatingLevel = true;
        titleOptions.setActive(false);
    } else if (titleOptionsSelection == 2) {
        selectingLevel = true;
        titleOptions.setActive(false);
        levelSelector.setActive(true);
        
        updateAvailableLevels();
    } else if (titleOptionsSelection == 3) {
        return false;
    }
    
    return true;
}

void updateAvailableLevels() {
    for (auto const& dir_entry : std::filesystem::directory_iterator("levels")) {
        if (dir_entry.path().extension() == ".lvl") {
            availableLevels.push_back(dir_entry.path().filename().replace_extension(""));
            levelSelector.addOption((availableLevels.end()-1)->string(), 0xFF, 0xFF, 0xFF, 0xFF);
        }
    }
}

void resetLevel() {
    player.setPos(level.getStartX(), level.getStartY());
    player.stop();
    player.destroyRope();
    player.destroyGrappleSeeker();
    
    startTicks = SDL_GetTicks64();
}
