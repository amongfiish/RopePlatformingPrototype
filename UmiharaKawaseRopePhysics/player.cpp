#include "player.hpp"

const double JUMP_VELOCITY = 4;
const double AIR_BLAST_VELOCITY_X = 2;
const double AIR_BLAST_VELOCITY_Y = 4;

const double GRAVITY = 0.2;
const double SWING_SLOWDOWN = 0.005;  // air resistance
const double GROUND_FRICTION = 0.1;

const double AIR_ACCELERATION = 0.03;
const double GROUND_ACCELERATION = 0.2;
const double MAX_GROUND_VELOCITY = 2;
const double MAX_AIR_VELOCITY = 2;

const int LEVEL_BOTTOM = 484;

Player::Player() {
    _x = 0;
    _x = 0;
    _width = PLAYER_WIDTH;
    _height = PLAYER_HEIGHT;
    
    _grappleSeeker = NULL;
    _rope = NULL;
    
    _grounded = false;
    _facing = RIGHT;
    
    _canAirBlast = false;
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

Rope *Player::getRope() {
    return _rope;
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
    
    if (alignedX && alignedY && !wasAlignedX && !wasAlignedY) {
        if (movingLeft) {
            return RIGHT;
        } else if (movingRight) {
            return LEFT;
        } else if (movingUp) {
            return DOWN;
        } else if (movingDown) {
            return UP;
        }
    } else if (alignedX && alignedY && !wasAlignedY && wasAlignedX) {
        if (movingDown) {
            return UP;
        } else if (movingUp) {
            return DOWN;
        }
    } else if (alignedY && alignedX && !wasAlignedX && wasAlignedY) {
        if (movingRight) {
            return LEFT;
        } else if (movingLeft) {
            return RIGHT;
        }
    }
    
    return -1;
}

bool Player::update(KeyboardLayout *keys, Level *level) {
    _velocityY += GRAVITY;
    
    // fire direction (aim)
    int aim = -1;
    if (keys->getUpState() != NONE) {
        aim = UP;
    }
    if (keys->getDownState() != NONE) {
        aim = DOWN;
    }
    if (keys->getUpState() != NONE && keys->getLeftState() != NONE) {
        aim = UPLEFT;
    }
    if (keys->getUpState() != NONE && keys->getRightState() != NONE) {
        aim = UPRIGHT;
    }
    if (keys->getDownState() != NONE && keys->getLeftState() != NONE) {
        aim = DOWNLEFT;
    }
    if (keys->getDownState() != NONE && keys->getRightState() != NONE) {
        aim = DOWNRIGHT;
    }
    
    // movement
    if (aim < 0) {
        if (_grounded) {
            if (keys->getRightState() != NONE) {
                if (_velocityX < MAX_GROUND_VELOCITY) {
                    _velocityX += GROUND_ACCELERATION;
                }
                _facing = RIGHT;
            }
            if (keys->getLeftState() != NONE) {
                if (_velocityX > -MAX_GROUND_VELOCITY) {
                    _velocityX -= GROUND_ACCELERATION;
                }
                _facing = LEFT;
            }
        } else {
            if (keys->getRightState() != NONE) {
                if (_velocityX < MAX_AIR_VELOCITY) {
                    _velocityX += AIR_ACCELERATION;
                }
                _facing = RIGHT;
            }
            if (keys->getLeftState() != NONE) {
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
    
    // jump and air blast
    if (keys->getJumpState() == PRESSED && _grounded) {
        _velocityY -= JUMP_VELOCITY;
    } else if (keys->getAirBlastState() == PRESSED && !_grounded && _canAirBlast && !_rope) {
        _canAirBlast = false;
        switch (aim) {
            case UPLEFT:
                _velocityX += AIR_BLAST_VELOCITY_X;
                _velocityY += AIR_BLAST_VELOCITY_Y;
                break;
            case UP:
                _velocityY += AIR_BLAST_VELOCITY_Y;
                break;
            case UPRIGHT:
                _velocityX -= AIR_BLAST_VELOCITY_X;
                _velocityY += AIR_BLAST_VELOCITY_Y;
                break;
            case DOWNLEFT:
                _velocityX += AIR_BLAST_VELOCITY_X;
                _velocityY -= AIR_BLAST_VELOCITY_Y;
                break;
            case DOWN:
                _velocityY -= AIR_BLAST_VELOCITY_Y;
                break;
            case DOWNRIGHT:
                _velocityX -= AIR_BLAST_VELOCITY_X;
                _velocityY -= AIR_BLAST_VELOCITY_Y;
                break;
            default:
                if (_facing == LEFT) {
                    _velocityX += AIR_BLAST_VELOCITY_X;
                } else if (_facing == RIGHT) {
                    _velocityX -= AIR_BLAST_VELOCITY_X;
                }
                break;
        }
    }
    
    // stuff that needs doing on the ground
    if (_grounded) {
        _canAirBlast = true;
        
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
    }
    
    int collision = -1;
    _grounded = false;
    for (int i = 0; i < level->getNumberOfPlatforms(); i++) {
        collision = checkCollision(level->getPlatform(i));
        switch (collision) {
            case UP:
                _velocityY = 0;
                _y = level->getPlatform(i)->getY() - _width;
                _grounded = true;
                break;
            case DOWN:
                _velocityY = 0;
                _y = level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight();
                break;
            case LEFT:
                _velocityX = 0;
                _x = level->getPlatform(i)->getX() - _width;
                break;
            case RIGHT:
                _velocityX = 0;
                _x = level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth();
                break;
            default:
                break;
        }
    }
    
    _x += _velocityX;
    _y += _velocityY;
    
//    if (_grappleSeeker) {
//        _grappleSeeker->addVelocityX(_velocityX);
//        _grappleSeeker->addVelocityY(_velocityY);
//    }
        
    // rope creation and destruction
    if (keys->getGrappleState() != NONE) {
        if (_rope) {
            if (aim == UP || aim == DOWN) {
                if (keys->getDownState() != NONE) {
                    _rope->decreaseSlack();
                }
                
                if (keys->getUpState() != NONE) {
                    _rope->increaseSlack();
                }
            }
            
            _rope->update(level);
            _canAirBlast = true;
            
            _velocityX += _rope->getAccelerationX();
            _velocityY += _rope->getAccelerationY();
        } else if (!_grappleSeeker && keys->getGrappleState() == PRESSED) {
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
        if (_grappleSeeker->seek(level)) {
            destroyGrappleSeeker();
        }
    }
    
    if (_y > LEVEL_BOTTOM) {
        _x = level->getStartX();
        _y = level->getStartY();
        stop();
        
        if (_rope) {
            destroyRope();
        } else if (_grappleSeeker) {
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
