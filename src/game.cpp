#include <filesystem>
#include <vector>

#include "game.hpp"
#include "player.hpp"
#include "level.hpp"
#include "text.hpp"
using namespace std;

const string VERSION = "indev 9 (on hold)";

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
TextBox levelName;
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
bool timerStarted;

// in game text
TextBox timer;
ColorBlock timerBackground;
int maxTimerWidth;

int editorCursorX = MAP_WIDTH / 2;
int editorCursorY = MAP_HEIGHT / 2;

double cameraX;
double cameraY;
const int CAMERA_WIDTH = MAP_WIDTH * PLATFORM_WIDTH - 1;
const int CAMERA_HEIGHT = MAP_HEIGHT * PLATFORM_HEIGHT - 1;

double editorCameraX;
double editorCameraY;

const int RETURN_FRAMES = 15;
double returnVelocityX;
double returnVelocityY;

void updateLevelEditor(KeyboardLayout *keys);
bool updateMenu(KeyboardLayout *keys, char pressedLetters[], int numPressedLetters);
void resetLevel(bool animate);

void updateAvailableLevels();

bool gameInit() {
    filesystem::path levels("levels");
    
    if (!filesystem::exists(levels)) {
        filesystem::create_directory(levels);
    }
    
    // menu text
    title.setText("Grappling Hook Prototype");
    title.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    title.setX(10);
    title.setY(0);
    title.initFont(64);
    
    titleOptions.setPos(32, 100);
    titleOptions.setActive(true);
    titleOptions.setScrollable(false);
    titleOptions.setFontSize(24);
    titleOptions.addOption("NEW LEVEL", 0xFF, 0xFF, 0xFF, 0xFF);
    titleOptions.addOption("LOAD LEVEL", 0xFF, 0xFF, 0xFF, 0xFF);
    titleOptions.addOption("QUIT", 0xFF, 0xFF, 0xFF, 0xFF);
    
    versionIndicator.setText(VERSION);
    versionIndicator.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    versionIndicator.setX(690);
    versionIndicator.setY(450);
    versionIndicator.initFont(18);
    
    levelNameIndicator.setText("Level name:");
    levelNameIndicator.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    levelNameIndicator.setX(350);
    levelNameIndicator.setY(115);
    levelNameIndicator.initFont(24);
    
    newLevelName.setX(350);
    newLevelName.setY(150);
    newLevelName.setTextColor(0x00, 0x00, 0x00, 0xFF);
    newLevelName.setBackgroundColor(0xFF, 0xFF, 0xFF, 0xFF);
    newLevelName.setBackgroundWidth(200);
    newLevelName.setBackgroundHeight(40);
    newLevelName.setTextOffset(5);
    newLevelName.setFontSize(24);
    
    levelSelector.setPos(350, 200);
    levelSelector.setActive(true);
    levelSelector.setScrollable(true);
    levelSelector.setItemsToDisplay(3);
    levelSelector.setFontSize(24);
    
    // level end text
    winIndicator.setText("Level End");
    winIndicator.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    winIndicator.setX(250);
    winIndicator.setY(100);
    winIndicator.initFont(48);
    
    levelName.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    levelName.setX(250);
    levelName.setY(170);
    levelName.initFont(24);
    
    timeIndicator.setColor(0xFF, 0xFF, 0x00, 0xFF);
    timeIndicator.setX(250);
    timeIndicator.setY(210);
    timeIndicator.initFont(24);
    
    fastestIndicator.setColor(0xFF, 0xFF, 0x00, 0xFF);
    fastestIndicator.setX(250);
    fastestIndicator.setY(250);
    fastestIndicator.initFont(24);
    
    endOptions.setPos(250, 290);
    endOptions.setActive(true);
    endOptions.setScrollable(false);
    endOptions.setFontSize(24);
    endOptions.addOption("RETRY", 0xFF, 0xFF, 0xFF, 0xFF);
    endOptions.addOption("EDIT", 0xFF, 0xFF, 0xFF, 0xFF);
    endOptions.addOption("RESET FASTEST", 0xFF, 0xFF, 0xFF, 0xFF);
    endOptions.addOption("MAIN MENU", 0xFF, 0xFF, 0xFF, 0xFF);
    
    // pause text
    pauseIndicator.setText("IN-GAME MENU");
    pauseIndicator.setX(250);
    pauseIndicator.setY(100);
    pauseIndicator.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    pauseIndicator.initFont(48);
    
    pauseOptions.setPos(250, 170);
    pauseOptions.setActive(true);
    pauseOptions.setScrollable(false);
    pauseOptions.setFontSize(24);
    pauseOptions.addOption("RESUME", 0xFF, 0xFF, 0xFF, 0xFF);
    pauseOptions.addOption("RETRY", 0xFF, 0xFF, 0xFF, 0xFF);
    pauseOptions.addOption("MAIN MENU", 0xFF, 0xFF, 0xFF, 0xFF);
    
    // editor text
    editorIndicator.setText("EDITOR");
    editorIndicator.setColor(0x00, 0xFF, 0x00, 0xFF);
    editorIndicator.setX(10);
    editorIndicator.setY(5);
    editorIndicator.initFont(24);
    
    editorMode.setColor(0x00, 0xFF, 0x00, 0xFF);
    editorMode.setX(10);
    editorMode.setY(42);
    editorMode.initFont(24);
    
    platformType.setColor(0x00, 0xFF, 0x00, 0xFF);
    platformType.setX(10);
    platformType.setY(79);
    platformType.initFont(24);
    
    // game text
    timer.setColor(0xFF, 0xFF, 0x00, 0xFF);
    timer.setX(5);
    timer.setY(2);
    timer.initFont(24);
    
    timerBackground.setX(0);
    timerBackground.setY(0);
    timerBackground.setWidth(100);
    timerBackground.setHeight(32);
    timerBackground.setColor(0x00, 0x00, 0x00, 0x77);
    
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
            level.correctLevel();
            level.setFastestTime(-1);
            level.saveLevel(levelFilename);
            
            currentGameState = GAME;
            
            resetLevel(false);
        }
    }
    
    if (currentGameState == GAME) {
        char timerText[50];
        if (timerStarted) {
            snprintf(timerText, 50, "%.3f", (SDL_GetTicks64() - startTicks) / 1000.0);
        } else {
            snprintf(timerText, 50, "%.3f", 0.0);
            if (player.getX() != level.getStartX() || player.getY() != level.getStartY() || player.isGrappling()) {
                timerStarted = true;
                startTicks = SDL_GetTicks64();
            }
        }
        timer.setText(timerText);
        if (timer.getWidth() > maxTimerWidth) {
            maxTimerWidth = timer.getWidth();
        }
        
        if (keys->getPauseState() == PRESSED) {
            currentGameState = PAUSE;
        }
        
        if (keys->getResetState() == PRESSED && timerStarted) {
            resetLevel(true);
            return true;
        }
        
        if (level.collideEndX(player.getX(), player.getY(), player.getWidth(), player.getHeight())) {
            currentGameState = LEVEL_END;
            secondsTaken = (SDL_GetTicks64() - startTicks) / 1000.0;
            levelName.setText(levelFilename);
            
            bool newFastest = false;
            if (secondsTaken < level.getFastestTime() || level.getFastestTime() < 0) {
                level.setFastestTime(secondsTaken);
                level.saveLevel(levelFilename);
                newFastest = true;
            }
            
            char s[50];
            snprintf(s, 50, "Time: %.3f secs", secondsTaken);
            
            timeIndicator.setText(s);
            
            if (!newFastest) {
                snprintf(s, 50, "Fastest: %.3f secs", level.getFastestTime());
            } else {
                snprintf(s, 50, "Fastest: %.3f secs *NEW*", level.getFastestTime());
            }
                
            fastestIndicator.setText(s);
            fastestIndicator.detectWidth();
        }
        
        if (!player.update(keys, &level)) {
            resetLevel(true);
            return true;
        }
    } else if (currentGameState == PAUSE) {
        int pauseSelection = pauseOptions.update(keys);
        
        if (keys->getPauseState() == PRESSED) {
            currentGameState = GAME;
            pauseOptions.resetSelection();
        }
        
        if (pauseSelection == 0) {
            pauseOptions.resetSelection();
            currentGameState = GAME;
        } else if (pauseSelection == 1) {
            resetLevel(false);
            pauseOptions.resetSelection();
            currentGameState = GAME;
        } else if (pauseSelection == 2) {
            pauseOptions.resetSelection();
            
            level.saveLevel(levelFilename);
            currentGameState = MENU;
            level.resetLevel();
            
            resetLevel(false);
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
            resetLevel(false);
            currentGameState = GAME;
            endOptions.resetSelection();
        } else if (endOption == 1) {
            resetLevel(false);
            currentGameState = LEVEL_EDITOR;
            endOptions.resetSelection();
        } else if (endOption == 2) {
            level.setFastestTime(-1);
            level.saveLevel(levelFilename);
            fastestIndicator.setText("Fastest: no data");
            fastestIndicator.detectWidth();
            endOptions.resetSelection();
        } else if (endOption == 3) {
            level.saveLevel(levelFilename);
            level.resetLevel();
            currentGameState = MENU;
            endOptions.resetSelection();
        }
    } else if (currentGameState == LEVEL_RESET) {
        player.setPos(player.getX() + returnVelocityX, player.getY() + returnVelocityY);
        
        if (abs(player.getX() - level.getStartX()) <= abs(returnVelocityX) &&
            abs(player.getY() - level.getStartY()) <= abs(returnVelocityY)) {
            player.setPos(level.getStartX(), level.getStartY());
            currentGameState = GAME;
        }
    }
    
    // camera
    if (currentGameState == GAME || currentGameState == LEVEL_RESET) {
        cameraX = player.getX() + player.getWidth() / 2 - CAMERA_WIDTH / 2;
        cameraY = player.getY() + player.getHeight() / 2 - CAMERA_HEIGHT / 2;
        
        // lock to map edges
        if (cameraX < 0) {
            cameraX = 0;
        } else if (cameraX + CAMERA_WIDTH > level.getMaxX()) {
            cameraX = level.getMaxX() - CAMERA_WIDTH;
        }
        
        if (cameraY < 0) {
            cameraY = 0;
        } else if (cameraY + CAMERA_HEIGHT > level.getMaxY()) {
            cameraY = level.getMaxY() - CAMERA_HEIGHT;
        }
    }
    
    return true;
}

void gameDraw(SDL_Renderer* renderer) {
    if (currentGameState == GAME) {
        level.draw(renderer, cameraX, cameraY);
        player.draw(renderer, cameraX, cameraY);
        timerBackground.setWidth(maxTimerWidth + 10);
        timerBackground.draw(renderer);
        timer.draw(renderer);
    } else if (currentGameState == PAUSE) {
        pauseIndicator.draw(renderer);
        pauseOptions.draw(renderer);
    } else if (currentGameState == LEVEL_EDITOR) {
        level.draw(renderer, cameraX, cameraY);
        
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
        SDL_Rect cursorRect = { editorCursorX * PLATFORM_WIDTH - 1 - static_cast<int>(cameraX), editorCursorY * PLATFORM_WIDTH - 1 - static_cast<int>(cameraY), PLATFORM_WIDTH + 2, PLATFORM_HEIGHT + 2 };
        SDL_RenderDrawRect(renderer, &cursorRect);
        
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_Rect startPosRect = { level.getStartX() - static_cast<int>(cameraX), level.getStartY() - static_cast<int>(cameraY), PLATFORM_WIDTH, PLATFORM_HEIGHT };
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
        levelName.draw(renderer);
        timeIndicator.draw(renderer);
        fastestIndicator.draw(renderer);
        endOptions.draw(renderer);
    } else if (currentGameState == LEVEL_RESET) {
        level.draw(renderer, cameraX, cameraY);
        player.draw(renderer, cameraX, cameraY);
    }
}

void updateLevelEditor(KeyboardLayout *keys) {
    if (keys->getResetState() == PRESSED) {
        editorCursorX = level.getStartX() / PLATFORM_WIDTH;
        editorCursorY = level.getStartY() / PLATFORM_HEIGHT;
        
        cameraX = level.getStartX() + PLATFORM_WIDTH / 2 - CAMERA_WIDTH / 2;
        cameraY = level.getStartY() + PLATFORM_HEIGHT / 2 - CAMERA_HEIGHT / 2;
    }
    
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
        currentLevelEditorMode = PLATFORM;
    }
    if (keys->getPreviousPlatformTypeState() == PRESSED) {
        currentLevelEditorMode = PLATFORM;
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
    
    if (editorCursorX * PLATFORM_WIDTH < cameraX) {
        cameraX = editorCursorX * PLATFORM_WIDTH;
    } else if (editorCursorX * PLATFORM_WIDTH > cameraX + CAMERA_WIDTH - 1) {
        cameraX = editorCursorX * PLATFORM_WIDTH - CAMERA_WIDTH + PLATFORM_WIDTH;
    }
    
    if (editorCursorY * PLATFORM_HEIGHT < cameraY) {
        cameraY = editorCursorY * PLATFORM_HEIGHT;
    } else if (editorCursorY * PLATFORM_HEIGHT > cameraY + CAMERA_HEIGHT - 1) {
        cameraY = editorCursorY * PLATFORM_HEIGHT - CAMERA_HEIGHT + PLATFORM_HEIGHT;
    }
    
//    if (editorCursorX < 0) {
//        editorCursorX = MAP_WIDTH - 1;
//    } else if (editorCursorX >= MAP_WIDTH) {
//        editorCursorX = 0;
//    }
//
//    if (editorCursorY < 0) {
//        editorCursorY = MAP_HEIGHT - 1;
//    } else if (editorCursorY >= MAP_HEIGHT) {
//        editorCursorY = 0;
//    }
    
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
                currentLevelEditorMode = PLATFORM;
            }
        } else if (currentLevelEditorMode == END_POINT) {
            if (platformExists < 0 &&
                !startPosHere) {
                
                level.setEndPos(editorCursorX * PLATFORM_WIDTH, editorCursorY * PLATFORM_HEIGHT);
                currentLevelEditorMode = PLATFORM;
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
                bool levelExists = false;
                updateAvailableLevels();
                for (int i = 0; i < availableLevels.size(); i++) {
                    if (newLevelName.getText() == availableLevels[i]) {
                        levelExists = true;
                    }
                }
                levelSelector.clearOptions();
                availableLevels.clear();
                
                if (!levelExists) {
                    levelFilename = newLevelName.getText();
                    
                    creatingLevel = false;
                    titleOptions.setActive(true);
                    
                    currentGameState = LEVEL_EDITOR;
                    
                    player.setPos(level.getStartX(), level.getStartY());
                    
                    level.setStartPos(32, (MAP_HEIGHT - 2) * PLATFORM_HEIGHT);
                    level.setEndPos(96, (MAP_HEIGHT - 2) * PLATFORM_HEIGHT);
                    level.setFastestTime(-1);
                    level.resetLevel();
                    
                    level.saveLevel(levelFilename);
                    
                    newLevelName.reset();
                }
                
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
            
            resetLevel(false);
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
    for (auto const& dir_entry : std::filesystem::directory_iterator("levels")) {
        if (dir_entry.path().extension() == ".lvl") {
            availableLevels.push_back(dir_entry.path().filename().replace_extension(""));
            levelSelector.addOption((availableLevels.end()-1)->string(), 0xFF, 0xFF, 0xFF, 0xFF);
        }
    }
}

void resetLevel(bool animate) {
    player.stop();
    player.destroyRope();
    player.destroyGrappleSeeker();
    
    timerStarted = false;

    if (animate) {
        returnVelocityX = (level.getStartX() - player.getX()) / RETURN_FRAMES;
        returnVelocityY = (level.getStartY() - player.getY()) / RETURN_FRAMES;

        currentGameState = LEVEL_RESET;
    } else {
        player.setPos(level.getStartX(), level.getStartY());
    }
    
    maxTimerWidth = 0;
}
