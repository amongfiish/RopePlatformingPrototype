#include <cmath>

#include "grapple.hpp"

const double STRETCH_ACCELERATION = 0.25;

Rope::Rope(Player *p, int gX, int gY) {
    _player = p;
    
    _grappleX = gX;
    _grappleY = gY;
    
    double diffX = _grappleX - (p->getX() + p->getWidth() / 2);
    double diffY = _grappleY - (p->getY() + p->getHeight() / 2);
    _ropeLength = sqrt(pow(diffX, 2) + pow(diffY, 2));
    _angle = atan2(diffY, diffX);
    _stretch = 0;
}

double Rope::getAccelerationX() {
    if (_stretch <= 0) {
        return 0;
    }
    
    double stretchFactor = _stretch * STRETCH_ACCELERATION;
    return -stretchFactor * cos(_angle);
}

double Rope::getAccelerationY() {
    if (_stretch <= 0) {
        return 0;
    }
    
    double stretchFactor = _stretch * STRETCH_ACCELERATION;
    return -stretchFactor * sin(_angle);
}

bool Rope::update() {
    double diffX = _grappleX - (_player->getX() + _player->getWidth() / 2);
    double diffY = _grappleY - (_player->getY() + _player->getHeight() / 2);
    _angle = atan2(diffY, diffX);
    _stretch = sqrt(pow(diffX, 2) + pow(diffY, 2)) - _ropeLength;
    
    return true;
}

void Rope::draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
    
    SDL_RenderDrawLine(renderer, static_cast<int>(_player->getX() + _player->getWidth() / 2), static_cast<int>(_player->getY() + _player->getHeight() / 2), _grappleX, _grappleY);
}
