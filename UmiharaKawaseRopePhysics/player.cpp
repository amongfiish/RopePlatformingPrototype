#include "player.hpp"

Player::Player() {
    _x = 0;
    _x = 0;
    _width = PLAYER_WIDTH;
    _height = PLAYER_HEIGHT;
}

double Player::getX() {
    return _x;
}

double Player::getY() {
    return _y;
}

int Player::getWidth() {
    return _width;
}

int Player::getHeight() {
    return _height;
}

int Player::checkCollision(Platform *p) {
    bool alignedX = false;
    bool alignedY = false;
    
    bool movingRight = false;
    bool movingLeft = false;
    bool movingUp = false;
    bool movingDown = false;
           
    if (_y + _velocityY + _height > p->getY() &&
        _y + _velocityY < p->getY() + p->getHeight()) {
        // player is aligned on y axis
        alignedY = true;
    }
    
    if (_x + _velocityX + _width > p->getX() &&
        _x + _velocityX < p->getX() + p->getWidth()) {
        // player is aligned on x axis
        alignedX = true;
    }
    
    if (_velocityX > 0) {
        // the player is moving right
        movingRight = true;
    } else if (_velocityX < 0) {
        // the player is moving left
        movingLeft = true;
    }
    
    if (_velocityY > 0) {
        // the player is moving down
        movingDown = true;
    } else if (_velocityY < 0) {
        // the player is moving up
        movingUp = true;
    }
    
    if (alignedX && alignedY) {
        if (movingDown) {
            return UP;
        }
        
        if (movingUp) {
            return DOWN;
        }
        
        if (movingRight) {
            return LEFT;
        }
        
        if (movingLeft) {
            return RIGHT;
        }
    }
    
    return -1;
}

bool Player::update(const Uint8 *keys, Platform *level, int numPlatforms) {
    
    return true;
}

void Player::draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    
    SDL_Rect rect = { static_cast<int>(_x), static_cast<int>(_y), _width, _height };
    SDL_RenderFillRect(renderer, &rect);
}
