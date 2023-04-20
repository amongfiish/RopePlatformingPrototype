#include <iostream>
#include <fstream>
#include <filesystem>

#include "level.hpp"

Platform::Platform() {
    _x = 0;
    _y = 0;
    _width = 0;
    _height = 0;
    
    _type = NORMAL;
}

Platform::Platform(int x, int y, int w, int h, int type) {
    _x = x;
    _y = y;
    _width = w;
    _height = h;
    
    _type = type;
}

int Platform::getX() {
    return _x;
}

int Platform::getY() {
    return _y;
}

int Platform::getWidth() {
    return _width;
}

int Platform::getHeight() {
    return _height;
}

void Platform::setX(int x) {
    _x = x;
}

void Platform::setY(int y) {
    _y = y;
}

void Platform::setWidth(int w) {
    _width = w;
}

void Platform::setHeight(int h) {
    _height = h;
}

void Platform::setPos(int x, int y) {
    _x = x;
    _y = y;
}

void Platform::setType(int type) {
    _type = type;
}

int Platform::getType() {
    return _type;
}

void Platform::draw(SDL_Renderer *renderer) {
    if (_type == NORMAL) {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    } else if (_type == METAL) {
        SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
    } else if (_type == ICE) {
        SDL_SetRenderDrawColor(renderer, 0xC0, 0xEE, 0xFF, 0xFF);
    }
        
    SDL_Rect rect = { _x, _y, _width, _height };
    SDL_RenderFillRect(renderer, &rect);
}

Level::Level() {
    _platforms = new Platform[10];
    _numberOfPlatforms = 0;
    _platformsCapacity = 10;
}

Level::~Level() {
    delete[] _platforms;
}

int Level::platformExists(int x, int y) {
    for (int i = 0; i < _numberOfPlatforms; i++) {
        if (_platforms[i].getX() == x && _platforms[i].getY() == y) {
            return i;
        }
    }
    
    return -1;
}

int Level::getStartX() {
    return _startX;
}

int Level::getStartY() {
    return _startY;
}

void Level::setStartPos(int x, int y) {
    _startX = x;
    _startY = y;
}

bool Level::collideEndX(int x, int y, int w, int h) {
    if (x + w >= _endX &&
        x < _endX + PLATFORM_WIDTH &&
        y + h >= _endY &&
        y < _endY + PLATFORM_HEIGHT) {
        
        return true;
    }
    
    return false;
}

int Level::getEndX() {
    return _endX;
}

int Level::getEndY() {
    return _endY;
}

void Level::setEndPos(int x, int y) {
    _endX = x;
    _endY = y;
}

void Level::addPlatform(int x, int y, int w, int h, int type) {
    _platforms[_numberOfPlatforms].setX(x);
    _platforms[_numberOfPlatforms].setY(y);
    _platforms[_numberOfPlatforms].setWidth(w);
    _platforms[_numberOfPlatforms].setHeight(h);
    _platforms[_numberOfPlatforms].setType(type);
    
    _numberOfPlatforms++;
    
    if (_numberOfPlatforms >= _platformsCapacity) {
        _platformsCapacity *= 2;
        
        Platform *newPlatforms = new Platform[_platformsCapacity];
        for (int i = 0; i < _numberOfPlatforms; i++) {
            newPlatforms[i].setX(_platforms[i].getX());
            newPlatforms[i].setY(_platforms[i].getY());
            newPlatforms[i].setWidth(_platforms[i].getWidth());
            newPlatforms[i].setHeight(_platforms[i].getHeight());
            newPlatforms[i].setType(_platforms[i].getType());
        }
        delete[] _platforms;
        
        _platforms = newPlatforms;
    }
}

void Level::removePlatform(int i) {
    for (int j = i + 1; j < _numberOfPlatforms; j++) {
        _platforms[j - 1] = _platforms[j];
    }
    _numberOfPlatforms--;
}

void Level::resetLevel() {
    int totalPlatforms = _numberOfPlatforms;
    for (int i = 0; i < totalPlatforms; i++) {
        removePlatform(0);
    }
}

Platform *Level::getPlatform(int i) {
    if (i >= 0 && i < _numberOfPlatforms) {
        return _platforms + i;
    }
    
    return NULL;
}

int Level::getNumberOfPlatforms() {
    return _numberOfPlatforms;
}

void Level::draw(SDL_Renderer *renderer) {
    for (int i = 0; i < _numberOfPlatforms; i++) {
        _platforms[i].draw(renderer);
    }
    
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
    SDL_Rect endPosRect = { _endX, _endY, PLATFORM_WIDTH, PLATFORM_HEIGHT };
    
    SDL_RenderFillRect(renderer, &endPosRect);
}

void Level::saveLevel(string filename) {
    filesystem::path filePath(filename);
    
    filePath.replace_extension("lvl");
    
    ofstream file;
    file.open(filePath.string());
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int platform = platformExists(x * PLATFORM_WIDTH, y * PLATFORM_HEIGHT);
            if (platform >= 0) {
                file.put(_platforms[platform].getType() + 51);
            } else if (_endX == x * PLATFORM_WIDTH && _endY == y * PLATFORM_HEIGHT) {
                file.put('2');
            } else if (_startX == x * PLATFORM_WIDTH && _startY == y * PLATFORM_HEIGHT) {
                file.put('1');
            } else {
                file.put('0');
            }
        }
    }
    
    file.close();
}

void Level::loadLevel(string filename) {
    ifstream file;
    file.open(filename);
    
    if (!file.fail()) {
        char currentPos;
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                file.get(currentPos);
                
                if (currentPos == '1') {
                    setStartPos(x * PLATFORM_WIDTH, y * PLATFORM_HEIGHT);
                } else if (currentPos == '2') {
                    setEndPos(x * PLATFORM_WIDTH, y * PLATFORM_WIDTH);
                } else if (currentPos > '2') {
                    addPlatform(x * PLATFORM_WIDTH, y * PLATFORM_HEIGHT, PLATFORM_WIDTH, PLATFORM_HEIGHT, currentPos - 51);
                }
            }
        }
    }
    
    file.close();
}
