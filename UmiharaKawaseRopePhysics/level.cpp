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
