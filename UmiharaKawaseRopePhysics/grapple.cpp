#include <cmath>

#include "grapple.hpp"

const double STRETCH_ACCELERATION = 0.2;

int Pivot::getX() {
    return _x;
}

int Pivot::getY() {
    return _y;
}

void Pivot::setX(int x) {
    _x = x;
}

void Pivot::setY(int y) {
    _y = y;
}

void Pivot::setAttachAngle(double attachAngle) {
    _attachAngle = attachAngle;
}

double Pivot::getAttachAngle() {
    return _attachAngle;
}

Rope::Rope(Player *p, int gX, int gY) {
    _player = p;
    
    _grappleX = gX;
    _grappleY = gY;
    
    _ropeLength = getCurrentLength();
    _angle = getCurrentAngle();
    _stretch = 0;
    
    // pivots
    _pivots = new Pivot[10];
    _numberOfPivots = 0;
    _pivotsCapacity = 10;
}

double Rope::getAccelerationX() {
    if (_stretch <= 0) {
        return 0;
    }
    
    double stretchFactor = _stretch * STRETCH_ACCELERATION;
    return stretchFactor * cos(_angle);
}

double Rope::getAccelerationY() {
    if (_stretch <= 0) {
        return 0;
    }
    
    double stretchFactor = _stretch * STRETCH_ACCELERATION;
    return stretchFactor * sin(_angle);
}

double Rope::getCurrentLength() {
    if (_numberOfPivots == 0) {
        double diffX = _grappleX - (_player->getX() + _player->getWidth() / 2);
        double diffY = _grappleY - (_player->getY() + _player->getHeight() / 2);
        
        return sqrt(pow(diffX, 2) + pow(diffY, 2));
    }
    
    double sum = 0;
    double diffX = _grappleX - _pivots[0].getX();
    double diffY = _grappleY - _pivots[0].getY();
    sum += sqrt(pow(diffX, 2) + pow(diffY, 2));
    
    for (int i = 1; i < _numberOfPivots - 1; i++) {
        diffX = _pivots[i - 1].getX() - _pivots[i].getX();
        diffY = _pivots[i - 1].getY() - _pivots[i].getY();
        sum += sqrt(pow(diffX, 2) + pow(diffY, 2));
    }
        
    diffX = _pivots[_numberOfPivots - 1].getX() - (_player->getX() + _player->getWidth() / 2);
    diffY = _pivots[_numberOfPivots - 1].getY() - (_player->getY() + _player->getHeight() / 2);
    sum += sqrt(pow(diffX, 2) + pow(diffY, 2));
    
//    printf("Current Rope Length: %f\n", sum);
//    printf("Real Rope Length: %f\n", _ropeLength);
//    printf("Number of Pivots: %d\n", _numberOfPivots);
    return sum;
}

double Rope::getCurrentAngle() {
    double diffX;
    double diffY;
    
    if (_numberOfPivots > 0) {
        diffX = _pivots[_numberOfPivots - 1].getX() - (_player->getX() + _player->getWidth() / 2);
        diffY = _pivots[_numberOfPivots - 1].getY() - (_player->getY() + _player->getHeight() / 2);
    } else {
        diffX = _grappleX - (_player->getX() + _player->getWidth() / 2);
        diffY = _grappleY - (_player->getY() + _player->getHeight() / 2);
    }
        
    return atan2(diffY, diffX);
}

int Rope::collideCorners(Platform *level, int numPlatforms) {
    double diffX;
    double diffY;
    
    if (_numberOfPivots > 0) {
        diffX = _pivots[_numberOfPivots - 1].getX() - (_player->getX() + _player->getWidth() / 2);
        diffY = _pivots[_numberOfPivots - 1].getY() - (_player->getY() + _player->getHeight() / 2);
//        printf("Pivot X: %d, Pivot Y: %d\n", _pivots[_numberOfPivots - 1].getX(), _pivots[_numberOfPivots - 1].getY());
    } else {
        diffX = _grappleX - (_player->getX() + _player->getWidth() / 2);
        diffY = _grappleY - (_player->getY() + _player->getHeight() / 2);
    }
        
    double slope = diffY / diffX;
    double x = _player->getX() + _player->getWidth() / 2;
    double y = _player->getY() + _player->getHeight() / 2;

    bool looping = true;
    while (looping) {
        if (diffY > 0) {
            if (_numberOfPivots > 0) {
                if (y > _pivots[_numberOfPivots - 1].getY()) {
                    break;
                }
            } else {
                if (y > _grappleY) {
                    break;
                }
            }
        } else if (diffY < 0) {
            if (_numberOfPivots > 0) {
                if (y < _pivots[_numberOfPivots - 1].getY()) {
                    break;
                }
            } else {
                if (y < _grappleY) {
                    break;
                }
            }
        }
        
        for (int i = 0; i < numPlatforms; i++) {
            if (x >= level[i].getX() && x <= level[i].getX() + level[i].getWidth() &&
                y >= level[i].getY() && y <= level[i].getY() + level[i].getHeight()) {
                int direction = -1;
                
                // collision
                if ((_angle >= M_PI_4 && _angle <= 3 * M_PI_4) || (_angle <= -M_PI_4 && _angle >= -3 * M_PI_4)) {
                    // up/down
                    if (_player->getVelocityX() > 0) {
                        direction = RIGHT;
                    } else if (_player->getVelocityX() < 0) {
                        direction = LEFT;
                    }
                } else if ((_angle >= -M_PI_4 && _angle <= 0) || (_angle >= 0 && _angle <= M_PI_4) || (_angle >= 3 * M_PI_4 && _angle <= M_PI) || (_angle <= -3 * M_PI_4 && _angle >= -M_PI)) {
                    // left/right
                    if (_player->getVelocityY() > 0) {
                        direction = DOWN;
                    } else if (_player->getVelocityY() < 0) {
                        direction = UP;
                    }
                }
                
//                printf("Direction: %d, Angle: %f\n", direction, _angle);
                
                if (direction >= 0) {
                    if ((_angle < M_PI_2 && _angle > 0) || (_angle > -M_PI && _angle < -M_PI_2)) {
                        // up/left and down/right
                        if (direction == UP || direction == RIGHT) {
                            // moving 'right'
                            _pivots[_numberOfPivots].setX(level[i].getX());
                            _pivots[_numberOfPivots].setY(level[i].getY() + level[i].getHeight());
                        } else if (direction == DOWN || direction == LEFT) {
                            // moving 'left'
                            _pivots[_numberOfPivots].setX(level[i].getX() + level[i].getWidth());
                            _pivots[_numberOfPivots].setY(level[i].getY());
                        }
                    } else if ((_angle > M_PI_2 && _angle < M_PI) || (_angle > -M_PI_2 && _angle < 0)) {
                        // up/right and down/left
                        if (direction == DOWN || direction == RIGHT) {
                            // moving 'right'
                            _pivots[_numberOfPivots].setX(level[i].getX());
                            _pivots[_numberOfPivots].setY(level[i].getY());
                        } else if (direction == UP || direction == LEFT) {
                            // moving 'left'
                            _pivots[_numberOfPivots].setX(level[i].getX() + level[i].getWidth());
                            _pivots[_numberOfPivots].setY(level[i].getY() + level[i].getHeight());
                        }
                    }
                    
                    if (_numberOfPivots) {
                        diffX = _pivots[_numberOfPivots - 1].getX() - _pivots[_numberOfPivots].getX();
                        diffY = _pivots[_numberOfPivots - 1].getY() - _pivots[_numberOfPivots].getY();
                    } else {
                        diffX = _grappleX - _pivots[_numberOfPivots].getX();
                        diffY = _grappleY - _pivots[_numberOfPivots].getY();
                    }
                    
                    printf("Diff X: %f, Diff Y: %f, New Attach Angle: %f\n", diffX, diffY, atan2(diffY, diffX));
                    
                    _pivots[_numberOfPivots].setAttachAngle(atan2(diffY, diffX));
                    
                    // can't determine direction based on speed, due to stretch/bounce. solutions: average velocity? new quadrant definition? <- interesting...
                    // sometimes the new pivot's properties aren't set if the above conditions aren't met, leading to a pivot being 'created' at x=0 y=0. see above comment
                    _numberOfPivots++;
                }
                
                if (_numberOfPivots >= _pivotsCapacity) {
                    _pivotsCapacity *= 2;
                    Pivot *newPivots = new Pivot[_pivotsCapacity];
                    for (int i = 0; i < _numberOfPivots; i++) {
                        newPivots[i].setX(_pivots[i].getX());
                        newPivots[i].setY(_pivots[i].getY());
                    }
                    delete[] _pivots;
                    
                    _pivots = newPivots;
                }
                
                looping = false;
                break;
            }
        }
        
        y += (diffY > 0) ? abs(slope) : -abs(slope);
        x += (diffX > 0) ? 1 : -1;
    }
    
    return -1;
}

bool Rope::update(Platform *level, int numPlatforms) {
    _previousAngle = _angle;
    _angle = getCurrentAngle();
    _stretch = getCurrentLength() - _ropeLength;
    
    if (_numberOfPivots) {
        if ((_pivots[_numberOfPivots - 1].getAttachAngle() <= getCurrentAngle() && _pivots[_numberOfPivots - 1].getAttachAngle() >= _previousAngle) ||
            (_pivots[_numberOfPivots - 1].getAttachAngle() >= getCurrentAngle() && _pivots[_numberOfPivots - 1].getAttachAngle() <= _previousAngle)) {
            // DDEESSTTRROOYY current pivot
            _numberOfPivots--;
        }
    }
    collideCorners(level, numPlatforms);
    
    return true;
}

void Rope::draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
    
    if (_numberOfPivots > 0) {
        SDL_RenderDrawLine(renderer, _grappleX, _grappleY, _pivots[0].getX(), _pivots[0].getY());
        for (int i = 1; i < _numberOfPivots - 1; i++) {
            SDL_RenderDrawLine(renderer, _pivots[i - 1].getX(), _pivots[i - 1].getY(), _pivots[i].getX(), _pivots[i].getY());
        }
        SDL_RenderDrawLine(renderer, _pivots[_numberOfPivots - 1].getX(), _pivots[_numberOfPivots - 1].getY(), _player->getX() + _player->getWidth() / 2, _player->getY() + _player->getHeight() / 2);
    } else {
        SDL_RenderDrawLine(renderer, static_cast<int>(_player->getX() + _player->getWidth() / 2), static_cast<int>(_player->getY() + _player->getHeight() / 2), _grappleX, _grappleY);
    }
}
