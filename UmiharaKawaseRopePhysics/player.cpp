#include "player.hpp"

#ifdef _WIN64
#define M_PI_2 M_PI/2
#define M_PI_4 M_PI/4
#endif

Player::Player() {
    _x = 0;
    _x = 0;
    _width = PLATFORM_WIDTH;
    _height = PLATFORM_HEIGHT;
    
    _grappleSeeker = NULL;
    _rope = NULL;
    
    _grounded = false;
    _facing = RIGHT;
    
    _canAirBlast = false;
    
    _wasCollidingVertically = false;
    _wasCollidingHorizontally = false;
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

Platform *Player::getGroundedPlatform() {
    return _groundedPlatform;
}

Rope *Player::getRope() {
    return _rope;
}

bool rectsOverlap(double x1, double y1, int w1, int h1, double x2, double y2, int w2, int h2) {
    if (x1 > x2 - w1 && x1 < x2 + w2 &&   // aligned x
        y1 > y2 - h1 && y1 < y2 + h2) {   // aligned y
        return true;
    }
    
    return false;
}

int Player::checkCollision(Platform *p) {
    if (rectsOverlap(_x + _velocityX, _y, _width, _height, p->getX(), p->getY(), p->getWidth(), p->getHeight())) {
        if (_velocityX > 0) {
            return LEFT;
        } else if (_velocityX < 0) {
            return RIGHT;
        }
    }
    
    if (rectsOverlap(_x, _y + _velocityY, _width, _height, p->getX(), p->getY(), p->getWidth(), p->getHeight())) {
        if (_velocityY > 0) {
            return UP;
        } else if (_velocityY < 0) {
            return DOWN;
        }
    }
    
    return -1;
}

bool Player::update(KeyboardLayout *keys, Level *level) {
    _velocityY += GRAVITY;
    
    // fire direction (aim)
    _aim = -1;
    if (keys->getUpState() != NONE) {
        _aim = UP;
    }
    if (keys->getDownState() != NONE) {
        _aim = DOWN;
    }
    if (keys->getUpState() != NONE && keys->getLeftState() != NONE) {
        _aim = UPLEFT;
    }
    if (keys->getUpState() != NONE && keys->getRightState() != NONE) {
        _aim = UPRIGHT;
    }
    if (keys->getDownState() != NONE && keys->getLeftState() != NONE) {
        _aim = DOWNLEFT;
    }
    if (keys->getDownState() != NONE && keys->getRightState() != NONE) {
        _aim = DOWNRIGHT;
    }
    
    
    
    // movement
    if (_aim < 0) {
        if (_grounded) {
            double acceleration = NORMAL_ACCELERATION;
            if (_groundedPlatform->getType() == ICE) {
                acceleration = ICE_ACCELERATION;
            }
            
            if (keys->getRightState() != NONE) {
                if (_velocityX < MAX_GROUND_VELOCITY) {
                    _velocityX += acceleration;
                }
                _facing = RIGHT;
            }
            if (keys->getLeftState() != NONE) {
                if (_velocityX > -MAX_GROUND_VELOCITY) {
                    _velocityX -= acceleration;
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
        switch (_aim) {
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
        
        double friction = NORMAL_FRICTION;
        if (_groundedPlatform->getType() == ICE) {
            friction = ICE_FRICTION;
        }
        
        // ground friction
        if ((_velocityX < 0 && _velocityX > -friction) ||
            (_velocityX > 0 && _velocityX < friction)) {
            _velocityX = 0;
        }
        
        if (_velocityX > 0) {
            _velocityX -= friction;
        } else if (_velocityX < 0) {
            _velocityX += friction;
        }
    }
    
    if (_velocityX > MAX_VELOCITY_X) {
        _velocityX = MAX_VELOCITY_X;
    } else if (_velocityX < -MAX_VELOCITY_X) {
        _velocityX = -MAX_VELOCITY_X;
    }
    
    if (_velocityY > MAX_VELOCITY_Y) {
        _velocityY = MAX_VELOCITY_Y;
    } else if (_velocityY < -MAX_VELOCITY_Y) {
        _velocityY = -MAX_VELOCITY_Y;
    }
    
    int collision = -1;
    _grounded = false;
    for (int i = 0; i < level->getNumberOfPlatforms(); i++) {
        collision = checkCollision(level->getPlatform(i));
        switch (collision) {
            case UP:
                _velocityY = 0;
                _y = level->getPlatform(i)->getY() - _height;
                _grounded = true;
                _groundedPlatform = level->getPlatform(i);
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
            if (_aim == UP || _aim == DOWN) {
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
            if (_aim == UPLEFT) {
                createGrappleSeeker(-3 * M_PI_4);
            } else if (_aim == UP) {
                createGrappleSeeker(-M_PI_2);
            } else if (_aim == UPRIGHT) {
                createGrappleSeeker(-M_PI_4);
            } else if (_aim == DOWNLEFT) {
                createGrappleSeeker(3 * M_PI_4);
            } else if (_aim == DOWN) {
                createGrappleSeeker(M_PI_2);
            } else if (_aim == DOWNRIGHT) {
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
    
    if ((_x + _width + 1 < 0 || _x > MAP_WIDTH * PLATFORM_WIDTH || _y + _height + 1 < 0 || _y > MAP_HEIGHT * PLATFORM_HEIGHT) && !_rope && !_grappleSeeker) {
        return false;
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
    
    // eyes whites
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    
    SDL_Rect leftWhiteRect = { static_cast<int>(_x) + LEFT_EYE_POS, static_cast<int>(_y) + EYE_HEIGHT, EYE_WIDTH, EYE_WIDTH };
    SDL_RenderFillRect(renderer, &leftWhiteRect);
    
    SDL_Rect rightWhiteRect = { static_cast<int>(_x) + RIGHT_EYE_POS, static_cast<int>(_y) + EYE_HEIGHT, EYE_WIDTH, EYE_WIDTH };
    SDL_RenderFillRect(renderer, &rightWhiteRect);
    
    // pupils
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    
    int lookXOffset = 0;
    int lookYOffset = 0;
    
    switch (_aim) {
        case UPLEFT:
            lookXOffset -= LOOK_DISTANCE;
            lookYOffset -= LOOK_DISTANCE;
            break;
        case UP:
            lookYOffset -= LOOK_DISTANCE;
            break;
        case UPRIGHT:
            lookXOffset += LOOK_DISTANCE;
            lookYOffset -= LOOK_DISTANCE;
            break;
        case DOWNLEFT:
            lookXOffset -= LOOK_DISTANCE;
            lookYOffset += LOOK_DISTANCE;
            break;
        case DOWN:
            lookYOffset += LOOK_DISTANCE;
            break;
        case DOWNRIGHT:
            lookXOffset += LOOK_DISTANCE;
            lookYOffset += LOOK_DISTANCE;
            break;
        default:
            if (_facing == LEFT) {
                lookXOffset -= LOOK_DISTANCE;
            } else if (_facing == RIGHT) {
                lookXOffset += LOOK_DISTANCE;
            }
            break;
    }
    
    SDL_Rect leftPupilRect = { leftWhiteRect.x + EYE_WIDTH / 2 - PUPIL_WIDTH / 2 + lookXOffset, leftWhiteRect.y + EYE_WIDTH / 2 - PUPIL_WIDTH / 2 + lookYOffset, PUPIL_WIDTH, PUPIL_WIDTH };
    SDL_Rect rightPupilRect = { rightWhiteRect.x + EYE_WIDTH / 2 - PUPIL_WIDTH / 2 + lookXOffset, rightWhiteRect.y + EYE_WIDTH / 2 - PUPIL_WIDTH / 2 + lookYOffset, PUPIL_WIDTH, PUPIL_WIDTH };
    
    SDL_RenderFillRect(renderer, &leftPupilRect);
    SDL_RenderFillRect(renderer, &rightPupilRect);
}
