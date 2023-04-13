#include <cmath>

#include "grapple.hpp"

const double STRETCH_ACCELERATION = 0.01;

// from the internet. see definition at the bottom of this file.
bool checkLineRectangleCollision(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh);
bool checkLineCollision(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

int Pivot::getX() {
    return _x;
}

int Pivot::getY() {
    return _y;
}

int Pivot::getDrawX() {
    return _drawX;
}

int Pivot::getDrawY() {
    return _drawY;
}

void Pivot::setX(int x) {
    _x = x;
}

void Pivot::setY(int y) {
    _y = y;
}

void Pivot::setDrawX(int x) {
    _drawX = x;
}

void Pivot::setDrawY(int y) {
    _drawY = y;
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
    
    for (int i = 1; i < _numberOfPivots; i++) {
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

    for (int i = 0; i < numPlatforms; i++) {
        double x1, y1, x2, y2;
        double rx, ry, rw, rh;
        
        if (_numberOfPivots == 0) {
            x1 = _grappleX;
            y1 = _grappleY;
        } else {
            x1 = _pivots[_numberOfPivots - 1].getX();
            y1 = _pivots[_numberOfPivots - 1].getY();
        }
        
        x2 = _player->getX() + _player->getWidth() / 2;
        y2 = _player->getY() + _player->getHeight() / 2;
        
        rx = level[i].getX();
        ry = level[i].getY();
        rw = level[i].getWidth();
        rh = level[i].getHeight();
        
        if (checkLineRectangleCollision(x1, y1, x2, y2, rx, ry, rw, rh)) {
            // collision
            int direction = -1;
            if ((_angle >= -M_PI && _angle <= -M_PI_4 && _previousAngle <= M_PI && _previousAngle >= M_PI_4) ||
                (_angle <= M_PI && _angle >= M_PI_4 && _previousAngle >= -M_PI && _previousAngle <= -M_PI_4)) {
                // crossing over from -pi to +pi and vice versa
                direction = LEFT;
            } else if (_angle >= 0 && _angle <= M_PI) {
                if (_angle > _previousAngle) {
                    direction = RIGHT;
                } else if (_angle < _previousAngle) {
                    direction = LEFT;
                }
            } else {
                if (_angle > _previousAngle) {
                    direction = LEFT;
                } else if (_angle < _previousAngle) {
                    direction = RIGHT;
                }
            }
            
            //                printf("Direction: %d, Angle: %f\n", direction, _angle);
            
            if (direction >= 0) {
                if ((_angle < M_PI_2 && _angle > 0) || (_angle < -M_PI_2)) {
                    // up/left and down/right
                    if (direction == UP || direction == RIGHT) {
                        // moving 'right'
                        _pivots[_numberOfPivots].setX(level[i].getX() - 2);
                        _pivots[_numberOfPivots].setY(level[i].getY() + level[i].getHeight() + 1);
                        _pivots[_numberOfPivots].setDrawX(level[i].getX() - 1);
                        _pivots[_numberOfPivots].setDrawY(level[i].getY() + level[i].getHeight());
                    } else if (direction == DOWN || direction == LEFT) {
                        // moving 'left'
                        _pivots[_numberOfPivots].setX(level[i].getX() + level[i].getWidth() + 1);
                        _pivots[_numberOfPivots].setY(level[i].getY() - 2);
                        _pivots[_numberOfPivots].setDrawX(level[i].getX() + level[i].getWidth());
                        _pivots[_numberOfPivots].setDrawY(level[i].getY() - 1);
                    }
                } else if ((_angle > M_PI_2) || (_angle > -M_PI_2 && _angle < 0)) {
                    // up/right and down/left
                    if (direction == DOWN || direction == RIGHT) {
                        // moving 'right'
                        _pivots[_numberOfPivots].setX(level[i].getX() - 2);
                        _pivots[_numberOfPivots].setY(level[i].getY() - 2);
                        _pivots[_numberOfPivots].setDrawX(level[i].getX() - 1);
                        _pivots[_numberOfPivots].setDrawY(level[i].getY() - 1);
                    } else if (direction == UP || direction == LEFT) {
                        // moving 'left'
                        _pivots[_numberOfPivots].setX(level[i].getX() + level[i].getWidth() + 1);
                        _pivots[_numberOfPivots].setY(level[i].getY() + level[i].getHeight() + 1);
                        _pivots[_numberOfPivots].setDrawX(level[i].getX() + level[i].getWidth());
                        _pivots[_numberOfPivots].setDrawY(level[i].getY() + level[i].getHeight());
                    }
                }
                
                if (_numberOfPivots) {
                    diffX = _pivots[_numberOfPivots - 1].getX() - _pivots[_numberOfPivots].getX();
                    diffY = _pivots[_numberOfPivots - 1].getY() - _pivots[_numberOfPivots].getY();
                } else {
                    diffX = _grappleX - _pivots[_numberOfPivots].getX();
                    diffY = _grappleY - _pivots[_numberOfPivots].getY();
                }
                
                //                    printf("Diff X: %f, Diff Y: %f, New Attach Angle: %f\n", diffX, diffY, atan2(diffY, diffX));
                
                _pivots[_numberOfPivots].setAttachAngle(atan2(diffY, diffX));
                
                printf("New X: %d, New Y: %d, New Attach Angle: %f\n", _pivots[_numberOfPivots].getX(), _pivots[_numberOfPivots].getY(), _pivots[_numberOfPivots].getAttachAngle());
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
        }
    }
    
    return -1;
}

bool Rope::update(Platform *level, int numPlatforms) {
    _previousAngle = _angle;
    _angle = getCurrentAngle();
    _stretch = getCurrentLength() - _ropeLength;
    
    // destroy the current pivot if and when it's passed again
    if (_numberOfPivots) {
        if (_pivots[_numberOfPivots - 1].getAttachAngle() > M_PI_2 &&  // top right sector
            ((_previousAngle > M_PI_2 && _previousAngle < _pivots[_numberOfPivots - 1].getAttachAngle() && getCurrentAngle() < -M_PI_2) ||
             (getCurrentAngle() > M_PI_2 && getCurrentAngle() < _pivots[_numberOfPivots - 1].getAttachAngle() && _previousAngle < -M_PI_2))) {
            _numberOfPivots--;
        } else if (_pivots[_numberOfPivots - 1].getAttachAngle() < -M_PI_2 &&  // bottom right sector
                   ((_previousAngle < -M_PI_2 && _previousAngle > _pivots[_numberOfPivots - 1].getAttachAngle() && getCurrentAngle() > M_PI_2) ||
                    (getCurrentAngle() < -M_PI_2 && getCurrentAngle() > _pivots[_numberOfPivots - 1].getAttachAngle() && _previousAngle > M_PI_2))) {
            _numberOfPivots--;
        } else if (((_pivots[_numberOfPivots - 1].getAttachAngle() < getCurrentAngle() && _pivots[_numberOfPivots - 1].getAttachAngle() > _previousAngle) ||
                    (_pivots[_numberOfPivots - 1].getAttachAngle() > getCurrentAngle() && _pivots[_numberOfPivots - 1].getAttachAngle() < _previousAngle)) &&
                   !((getCurrentAngle() > M_PI_2 && _previousAngle < -M_PI_2) || (_previousAngle > M_PI_2 && getCurrentAngle() < -M_PI_2))) {
            _numberOfPivots--;
        }
    }
    collideCorners(level, numPlatforms);
    
    return true;
}

void Rope::draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
    
    if (_numberOfPivots > 0) {
        SDL_RenderDrawLine(renderer, _grappleX, _grappleY, _pivots[0].getDrawX(), _pivots[0].getDrawY());
        for (int i = 1; i < _numberOfPivots; i++) {
            SDL_RenderDrawLine(renderer, _pivots[i - 1].getDrawX(), _pivots[i - 1].getDrawY(), _pivots[i].getDrawX(), _pivots[i].getDrawY());
        }
        SDL_RenderDrawLine(renderer, _pivots[_numberOfPivots - 1].getDrawX(), _pivots[_numberOfPivots - 1].getDrawY(), _player->getX() + _player->getWidth() / 2, _player->getY() + _player->getHeight() / 2);
    } else {
        SDL_RenderDrawLine(renderer, static_cast<int>(_player->getX() + _player->getWidth() / 2), static_cast<int>(_player->getY() + _player->getHeight() / 2), _grappleX, _grappleY);
    }
}

// collision code from https://www.jeffreythompson.org/collision-detection/line-rect.php
bool checkLineRectangleCollision(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh) {
    // check if the line has hit any of the rectangle's sides
    // uses the Line/Line function below
    bool left =   checkLineCollision(x1,y1,x2,y2, rx,ry,rx, ry+rh);
    bool right =  checkLineCollision(x1,y1,x2,y2, rx+rw,ry, rx+rw,ry+rh);
    bool top =    checkLineCollision(x1,y1,x2,y2, rx,ry, rx+rw,ry);
    bool bottom = checkLineCollision(x1,y1,x2,y2, rx,ry+rh, rx+rw,ry+rh);

    // if ANY of the above are true, the line
    // has hit the rectangle
    if (left || right || top || bottom) {
        return true;
    }
    
    return false;
}


bool checkLineCollision(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    // calculate the direction of the lines
    float uA = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
    float uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));

    // if uA and uB are between 0-1, lines are colliding
    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
        return true;
    }
    
    return false;
}
