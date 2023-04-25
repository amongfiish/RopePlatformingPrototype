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
    draw(renderer, 0, 0);
}

void Platform::draw(SDL_Renderer *renderer, double cameraX, double cameraY) {
    switch (_type) {
        case NORMAL:
            SDL_SetRenderDrawColor(renderer, NORMAL_COLOR.r, NORMAL_COLOR.g, NORMAL_COLOR.b, NORMAL_COLOR.a);
            break;
        case METAL:
            SDL_SetRenderDrawColor(renderer, METAL_COLOR.r, METAL_COLOR.g, METAL_COLOR.b, METAL_COLOR.a);
            break;
        case ICE:
            SDL_SetRenderDrawColor(renderer, ICE_COLOR.r, ICE_COLOR.g, ICE_COLOR.b, ICE_COLOR.a);
            break;
        case LAVA:
            SDL_SetRenderDrawColor(renderer, LAVA_COLOR.r, LAVA_COLOR.g, LAVA_COLOR.b, LAVA_COLOR.a);
            break;
    }
    
        
    SDL_Rect rect = { _x - static_cast<int>(cameraX), _y - static_cast<int>(cameraY), _width, _height };
    SDL_RenderFillRect(renderer, &rect);
}

Level::Level() {
    _platforms = new Platform[10];
    _numberOfPlatforms = 0;
    _platformsCapacity = 10;
    
    _startX = 0;
    _startY = 0;
    _endX = 64;
    _endY = 0;
    
    _maxX = MAP_WIDTH * PLATFORM_WIDTH;
    _maxY = MAP_HEIGHT * PLATFORM_HEIGHT;
    
    _fastestTime = -1;
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
    if (x + w - 1 >= _endX &&
        x < _endX + PLATFORM_WIDTH &&
        y + h - 1 >= _endY &&
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

int Level::getMaxX() {
    return _maxX;
}

int Level::getMaxY() {
    return _maxY;
}

void Level::correctLevel() {
    int minX = _maxX;
    int minY = _maxY;
    
    // determine minimum and maximum x and y
    for (int i = 0; i < _numberOfPlatforms; i++) {
        if (_platforms[i].getX() < minX) {
            minX = _platforms[i].getX();
        } else if (_platforms[i].getX() + _platforms[i].getWidth() - 1 > _maxX) {
            _maxX = _platforms[i].getX() + _platforms[i].getWidth() - 1;
        }
        
        if (_platforms[i].getY() < minY) {
            minY = _platforms[i].getY();
        } else if (_platforms[i].getY() + _platforms[i].getHeight() - 1 > _maxY) {
            _maxY = _platforms[i].getY() + _platforms[i].getHeight() - 1;
        }
    }
    
    if (_startX < minX) {
        minX = _startX;
    } else if (_startX + PLATFORM_WIDTH > _maxX) {
        _maxX = _startX + PLATFORM_WIDTH;
    }
    
    if (_startY < minY) {
        minY = _startY;
    } else if (_startY + PLATFORM_HEIGHT > _maxY) {
        _maxY = _startY + PLATFORM_HEIGHT;
    }
    
    if (_endX < minX) {
        minX = _endX;
    } else if (_endX + PLATFORM_WIDTH > _maxX) {
        _maxX = _endX + PLATFORM_WIDTH;
    }
    
    if (_endY < minY) {
        minY = _endY;
    } else if (_endY + PLATFORM_HEIGHT > _maxY) {
        _maxY = _endY + PLATFORM_HEIGHT;
    }
    
    // update level based on the new values
//    if (minX >= 0 && _maxX <= MAP_WIDTH * PLATFORM_WIDTH &&
//        minY >= 0 && _maxY <= MAP_HEIGHT * PLATFORM_HEIGHT) {
    for (int i = 0; i < _numberOfPlatforms; i++) {
        _platforms[i].setX(_platforms[i].getX() - minX);
        _platforms[i].setY(_platforms[i].getY() - minY);
    }
    
    _startX -= minX;
    _startY -= minY;
    
    _endX -= minX;
    _endY -= minY;
    
    _maxX -= minX;
    _maxY -= minY;
//    }
    
    if (_maxX < MAP_WIDTH * PLATFORM_WIDTH) {
        _maxX = MAP_WIDTH * PLATFORM_WIDTH;
    }
    
    if (_maxY < MAP_HEIGHT * PLATFORM_HEIGHT) {
        _maxY = MAP_HEIGHT * PLATFORM_HEIGHT;
    }
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

double Level::getFastestTime() {
    return _fastestTime;
}

void Level::setFastestTime(double fastestTime) {
    _fastestTime = fastestTime;
}

void Level::draw(SDL_Renderer *renderer) {
    draw(renderer, 0, 0);
}

void Level::draw(SDL_Renderer *renderer, double cameraX, double cameraY) {
    for (int i = 0; i < _numberOfPlatforms; i++) {
        _platforms[i].draw(renderer, cameraX, cameraY);
    }
    
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
    SDL_Rect endPosRect = { _endX - static_cast<int>(cameraX), _endY - static_cast<int>(cameraY), PLATFORM_WIDTH, PLATFORM_HEIGHT };
    
    SDL_RenderFillRect(renderer, &endPosRect);
}

void Level::saveLevel(string filename) {
    filesystem::path filePath("levels/" + filename);
    filePath.replace_extension("lvl");
    
    ofstream file;
    file.open(filePath.string());
    
    for (int y = 0; y < _maxY / PLATFORM_HEIGHT; y++) {
        for (int x = 0; x < _maxX / PLATFORM_HEIGHT; x++) {
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
        
        file.put('\n');
    }
    
    file.close();
    
    filePath.replace_extension("hs");
    file.open(filePath.string());
    
    file << to_string(getFastestTime()).c_str();
    
    file.close();
}

void Level::loadLevel(string filename) {
    filesystem::path filePath("levels/" + filename);
    filePath.replace_extension("lvl");
    
    ifstream file;
    file.open(filePath.string());
    
    if (!file.fail()) {
        char currentPos;
        int y = 0;
        while (y >= 0) {
            int x = 0;
            while (true) {
                file.get(currentPos);
                
                if (file.eof()) {
                    y = -2;
                    break;
                }
                
                if (currentPos == '\n') {
                    break;
                }
                
                if (currentPos == '1') {
                    setStartPos(x * PLATFORM_WIDTH, y * PLATFORM_HEIGHT);
                } else if (currentPos == '2') {
                    setEndPos(x * PLATFORM_WIDTH, y * PLATFORM_WIDTH);
                } else if (currentPos > '2') {
                    addPlatform(x * PLATFORM_WIDTH, y * PLATFORM_HEIGHT, PLATFORM_WIDTH, PLATFORM_HEIGHT, currentPos - 51);
                }
                
                x++;
            }
            
            y++;
        }
    }
    
    file.close();
    
    filePath.replace_extension("hs");
    file.open(filePath.string());
    
    string fastestTimeString;
    if (!file.fail()) {
        file >> fastestTimeString;
        _fastestTime = stod(fastestTimeString);
    } else {
        _fastestTime = -1;
    }
    
    file.close();
}
