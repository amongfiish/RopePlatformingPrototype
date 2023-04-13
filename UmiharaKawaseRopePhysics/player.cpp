#include "player.hpp"

const double JUMP_VELOCITY = 4;

const double GRAVITY = 0.2;
const double SWING_SLOWDOWN = 0.005;  // air resistance
const double GROUND_FRICTION = 0.1;

const double AIR_ACCELERATION = 0.03;
const double GROUND_ACCELERATION = 0.2;
const double MAX_GROUND_VELOCITY = 2;
const double MAX_AIR_VELOCITY = 2;

Player::Player() {
    _x = 0;
    _x = 0;
    _width = PLAYER_WIDTH;
    _height = PLAYER_HEIGHT;
    
    _grappleSeeker = NULL;
    _rope = NULL;
    
    _grounded = false;
    _facing = RIGHT;
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

double Player::getVelocityX() {
    return _velocityX;
}

double Player::getVelocityY() {
    return _velocityY;
}

void Player::setPos(int x, int y) {
    _x = x;
    _y = y;
}

void Player::stop() {
    _velocityX = 0;
    _velocityY = 0;
}

void Player::createGrappleSeeker(double angle) {
    _grappleSeeker = new GrappleSeeker(this, angle);
}

void Player::destroyGrappleSeeker() {
    delete _grappleSeeker;
    _grappleSeeker = NULL;
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
    
    int collision = -1;
    _grounded = false;
    for (int i = 0; i < numPlatforms; i++) {
        collision = checkCollision(level + i);
        switch (collision) {
            case UP:
                _velocityY = 0;
                _y = level[i].getY() - _width;
                _grounded = true;
                break;
            case DOWN:
                _velocityY = 0;
                _y = level[i].getY() + level[i].getHeight();
                break;
            case LEFT:
                _velocityX = 0;
                _x = level[i].getX() - _width;
                break;
            case RIGHT:
                _velocityX = 0;
                _x = level[i].getX() + level[i].getWidth();
                break;
            default:
                break;
        }
    }
    
    // fire direction (aim)
    int aim = -1;
    if (keys[SDL_SCANCODE_UP]) {
        aim = UP;
    }
    if (keys[SDL_SCANCODE_DOWN]) {
        aim = DOWN;
    }
    if (keys[SDL_SCANCODE_UP] && keys[SDL_SCANCODE_LEFT]) {
        aim = UPLEFT;
    }
    if (keys[SDL_SCANCODE_UP] && keys[SDL_SCANCODE_RIGHT]) {
        aim = UPRIGHT;
    }
    if (keys[SDL_SCANCODE_DOWN] && keys[SDL_SCANCODE_LEFT]) {
        aim = DOWNLEFT;
    }
    if (keys[SDL_SCANCODE_DOWN] && keys[SDL_SCANCODE_RIGHT]) {
        aim = DOWNRIGHT;
    }
    
    // movement
    if (aim < 0) {
        if (_grounded) {
            if (keys[SDL_SCANCODE_RIGHT]) {
                if (_velocityX < MAX_GROUND_VELOCITY) {
                    _velocityX += GROUND_ACCELERATION;
                }
                _facing = RIGHT;
            }
            if (keys[SDL_SCANCODE_LEFT]) {
                if (_velocityX > -MAX_GROUND_VELOCITY) {
                    _velocityX -= GROUND_ACCELERATION;
                }
                _facing = LEFT;
            }
        } else {
            if (keys[SDL_SCANCODE_RIGHT]) {
                if (_velocityX < MAX_AIR_VELOCITY) {
                    _velocityX += AIR_ACCELERATION;
                }
                _facing = RIGHT;
            }
            if (keys[SDL_SCANCODE_LEFT]) {
                if (_velocityX > -MAX_AIR_VELOCITY) {
                    _velocityX -= AIR_ACCELERATION;
                }
                _facing = LEFT;
            }
        }
    }
    
    // stuff that needs doing in the air
    if (!_grounded && _rope) {
        if (_velocityX > 0) {
            _velocityX -= SWING_SLOWDOWN;
        } else if (_velocityX < 0) {
            _velocityX += SWING_SLOWDOWN;
        }
    }
    
    // stuff that needs doing on the ground
    if (_grounded) {
        // ground friction
        if ((_velocityX < 0 && _velocityX > -GROUND_FRICTION) ||
            (_velocityX > 0 && _velocityX < GROUND_FRICTION)) {
            _velocityX = 0;
        }
        
        if (_velocityX > 0) {
            _velocityX -= GROUND_FRICTION;
        } else if (_velocityX < 0) {
            _velocityX += GROUND_FRICTION;
        }
        
        // jump
        if (keys[SDL_SCANCODE_C]) {
            _velocityY -= JUMP_VELOCITY;
        }
    }
    
    _x += _velocityX;
    _y += _velocityY;
    
    if (_grappleSeeker) {
        _grappleSeeker->addVelocityX(_velocityX);
        _grappleSeeker->addVelocityY(_velocityY);
    }
        
    // rope create and destruction
    if (keys[SDL_SCANCODE_X]) {
        if (_rope) {
            if (aim == UP || aim == DOWN) {
                if (keys[SDL_SCANCODE_DOWN]) {
                    _rope->decreaseSlack();
                }
                
                if (keys[SDL_SCANCODE_UP]) {
                    _rope->increaseSlack();
                }
            }
            
            _rope->update(level, numPlatforms);
            
            _velocityX += _rope->getAccelerationX();
            _velocityY += _rope->getAccelerationY();
        } else if (!_grappleSeeker) {
            if (aim == UPLEFT) {
                createGrappleSeeker(-3 * M_PI_4);
            } else if (aim == UP) {
                createGrappleSeeker(-M_PI_2);
            } else if (aim == UPRIGHT) {
                createGrappleSeeker(-M_PI_4);
            } else if (aim == DOWNLEFT) {
                createGrappleSeeker(3 * M_PI_4);
            } else if (aim == DOWN) {
                createGrappleSeeker(M_PI_2);
            } else if (aim == DOWNRIGHT) {
                createGrappleSeeker(M_PI_4);
            } else if (_facing == LEFT) {
                createGrappleSeeker(M_PI);
            } else if (_facing == RIGHT) {
                createGrappleSeeker(0);
            }
        }
    } else {
        if (_rope) {
            destroyRope();
        } else if (_grappleSeeker) {
            destroyGrappleSeeker();
        }
    }
    
    // update the seeker
    if (_grappleSeeker) {
        if (_grappleSeeker->seek(level, numPlatforms)) {
            destroyGrappleSeeker();
        }
    }
    
    return true;
}

void Player::draw(SDL_Renderer *renderer) {
    if (_rope) {
        _rope->draw(renderer);
    } else if (_grappleSeeker) {
        _grappleSeeker->draw(renderer);
    }
    
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    
    SDL_Rect rect = { static_cast<int>(_x), static_cast<int>(_y), _width, _height };
    SDL_RenderFillRect(renderer, &rect);
}
