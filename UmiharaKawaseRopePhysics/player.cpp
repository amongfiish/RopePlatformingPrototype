#include "player.hpp"

const double GRAVITY = 0.1;
const double SWING_SLOWDOWN = 0.003;  // air resistance

Player::Player() {
    _x = 0;
    _x = 0;
    _width = PLAYER_WIDTH;
    _height = PLAYER_HEIGHT;
    
    _rope = NULL;
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

void Player::setPos(int x, int y) {
    _x = x;
    _y = y;
}

void Player::stop() {
    _velocityX = 0;
    _velocityY = 0;
}

void Player::createRope(int gX, int gY) {
    _rope = new Rope(this, gX, gY);
}

void Player::destroyRope() {
    delete _rope;
    _rope = NULL;
}

int Player::checkCollision(Platform *p) {
    bool wasAlignedX = false;
    bool wasAlignedY = false;
    bool alignedX = false;
    bool alignedY = false;
    
    bool movingRight = false;
    bool movingLeft = false;
    bool movingUp = false;
    bool movingDown = false;
    
    if (_y + _height > p->getY() &&
        _y < p->getY() + p->getHeight()) {
        wasAlignedY = true;
    }
    
    if (_y + _velocityY + _height > p->getY() &&
        _y + _velocityY < p->getY() + p->getHeight()) {
        // player is aligned on y axis
        alignedY = true;
    }
    
    if (_x + _width > p->getX() &&
        _x < p->getX() + p->getWidth()) {
        wasAlignedX = true;
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
    
    if (alignedY && alignedX && !wasAlignedX) {
        if (movingRight) {
            return LEFT;
        } else if (movingLeft) {
            return RIGHT;
        }
    } else if (alignedX && alignedY && !wasAlignedY) {
        if (movingDown) {
            return UP;
        } else if (movingUp) {
            return DOWN;
        }
    }
    
    return -1;
}

bool Player::update(const Uint8 *keys, Platform *level, int numPlatforms) {
    _velocityY += GRAVITY;
    
    if (_rope) {
        _rope->update();
        
        printf("%f\n", _rope->getAccelerationX());
        _velocityX -= _rope->getAccelerationX();
        _velocityY -= _rope->getAccelerationY();
    }
    
    bool collided = false;
    for (int i = 0; i < numPlatforms; i++) {
        int collision = checkCollision(level + i);
        printf("%d\n", collision);
        switch (collision) {
            case UP:
                collided = true;
                _velocityY = 0;
                _y = level[i].getY() - _width;
                break;
            case DOWN:
                collided = true;
                _velocityY = 0;
                _y = level[i].getY() + level[i].getHeight();
                break;
            case LEFT:
                collided = true;
                _velocityX = 0;
                _x = level[i].getX() - _width;
                break;
            case RIGHT:
                collided = true;
                _velocityX = 0;
                _x = level[i].getX() + level[i].getWidth();
                break;
            default:
                break;
        }
    }
    
    if (!collided) {
        if (_velocityX > 0) {
            _velocityX -= SWING_SLOWDOWN;
        } else if (_velocityX < 0) {
            _velocityX += SWING_SLOWDOWN;
        }
    }
    
    _x += _velocityX;
    _y += _velocityY;
    
    return true;
}

void Player::draw(SDL_Renderer *renderer) {
    if (_rope) {
        _rope->draw(renderer);
    }
    
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    
    SDL_Rect rect = { static_cast<int>(_x), static_cast<int>(_y), _width, _height };
    SDL_RenderFillRect(renderer, &rect);
}
