#include "level.hpp"

Platform::Platform() {
    _x = 0;
    _y = 0;
    _width = 0;
    _width = 0;
}

Platform::Platform(int x, int y, int w, int h) {
    _x = x;
    _y = y;
    _width = w;
    _height = h;
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

void Platform::draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    
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

void save() {
    
}

void load() {
    
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

void Level::addPlatform(int x, int y, int w, int h) {
    _platforms[_numberOfPlatforms].setX(x);
    _platforms[_numberOfPlatforms].setY(y);
    _platforms[_numberOfPlatforms].setWidth(w);
    _platforms[_numberOfPlatforms].setHeight(h);
    
    _numberOfPlatforms++;
    
    if (_numberOfPlatforms >= _platformsCapacity) {
        _platformsCapacity *= 2;
        
        Platform *newPlatforms = new Platform[_platformsCapacity];
        for (int i = 0; i < _numberOfPlatforms; i++) {
            newPlatforms[i].setX(_platforms[i].getX());
            newPlatforms[i].setY(_platforms[i].getY());
            newPlatforms[i].setWidth(_platforms[i].getWidth());
            newPlatforms[i].setHeight(_platforms[i].getHeight());
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
}
