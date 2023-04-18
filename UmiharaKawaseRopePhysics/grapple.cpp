#include <cmath>

#include "grapple.hpp"
#include "player.hpp"

#ifdef _WIN64
#define M_PI_2 M_PI/2
#define M_PI_4 M_PI/4
#endif

// from the internet. see definition at the bottom of this file (modified)
CollisionReportContainer *getLineRectangleCollision(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh);
CollisionReport *getLineCollision(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
bool checkLineRectCollision(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh);
bool checkLineCollision(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

CollisionReport::CollisionReport() {
    _intersectionX = 0;
    _intersectionY = 0;
    
    _platform = NULL;
}

CollisionReport::CollisionReport(float x, float y, Platform *platform) {
    _intersectionX = x;
    _intersectionY = y;
    
    _platform = platform;
}

float CollisionReport::getIntersectionX() {
    return _intersectionX;
}

float CollisionReport::getIntersectionY() {
    return _intersectionY;
}

void CollisionReport::setIntersectionX(float x) {
    _intersectionX = x;
}

void CollisionReport::setIntersectionY(float y) {
    _intersectionY = y;
}

Platform *CollisionReport::getPlatform() {
    return _platform;
}

void CollisionReport::setPlatform(Platform *platform) {
    _platform = platform;
}

CollisionReportContainer::CollisionReportContainer() {
    _reports = new CollisionReport [10];
    _numberOfReports = 0;
    _reportsCapacity = 10;
}

CollisionReportContainer::~CollisionReportContainer() {
    delete[] _reports;
}

void CollisionReportContainer::addReport(float x, float y) {
    _reports[_numberOfReports].setIntersectionX(x);
    _reports[_numberOfReports].setIntersectionY(y);
    
    _numberOfReports++;
    if (_numberOfReports >= _reportsCapacity) {
        CollisionReport *newReports = new CollisionReport[_reportsCapacity * 2];
        for (int i = 0; i < _numberOfReports; i++) {
            newReports[i].setIntersectionX(_reports[i].getIntersectionX());
            newReports[i].setIntersectionY(_reports[i].getIntersectionY());
            newReports[i].setPlatform(_reports[i].getPlatform());
        }
        delete[] _reports;
        _reports = newReports;
        _reportsCapacity *= 2;
    }
}

CollisionReport *CollisionReportContainer::getReport(int i) {
    return _reports + i;
}

int CollisionReportContainer::getNumberOfReports() {
    return _numberOfReports;
}

GrappleSeeker::GrappleSeeker(Player *player, double angle) {
    _player = player;
    
    _angle = angle;
    
    _x = player->getX() + player->getWidth() / 2;
    _y = player->getY() + player->getHeight() / 2;
    _velocityX = SEEK_SPEED * cos(_angle);
    _velocityY = SEEK_SPEED * sin(_angle);
    
    _extending = true;
    
    _pivots = new Pivot[10];
    _numberOfPivots = 0;
    _pivotsCapacity = 10;
}

GrappleSeeker::~GrappleSeeker() {
    delete[] _pivots;
}

void GrappleSeeker::addVelocityX(double vX) {
    _velocityX += vX;
}

void GrappleSeeker::addVelocityY(double vY) {
    _velocityY += vY;
}

double GrappleSeeker::getCurrentLength() {
    if (_numberOfPivots == 0) {
        double diffX = _x - (_player->getX() + _player->getWidth() / 2);
        double diffY = _y - (_player->getY() + _player->getHeight() / 2);
        
        return sqrt(pow(diffX, 2) + pow(diffY, 2));
    }
    
    double sum = 0;
    double diffX = _x - _pivots[0].getX();
    double diffY = _y - _pivots[0].getY();
    sum += sqrt(pow(diffX, 2) + pow(diffY, 2));
    
    for (int i = 1; i < _numberOfPivots; i++) {
        diffX = _pivots[i - 1].getX() - _pivots[i].getX();
        diffY = _pivots[i - 1].getY() - _pivots[i].getY();
        sum += sqrt(pow(diffX, 2) + pow(diffY, 2));
    }
        
    diffX = _pivots[_numberOfPivots - 1].getX() - (_player->getX() + _player->getWidth() / 2);
    diffY = _pivots[_numberOfPivots - 1].getY() - (_player->getY() + _player->getHeight() / 2);
    sum += sqrt(pow(diffX, 2) + pow(diffY, 2));
    
    return sum;
}

CollisionReport *GrappleSeeker::collide(Platform *p) {
    float x1 = _x /*- _player->getVelocityX()*/;
    float y1 = _y /*- _player->getVelocityY()*/;
    float x2 = _x + _velocityX;
    float y2 = _y + _velocityY;
    
    float rx = p->getX();
    float ry = p->getY();
    float rw = p->getWidth();
    float rh = p->getHeight();
    
    CollisionReportContainer *collisions = getLineRectangleCollision(x1, y1, x2, y2, rx, ry, rw, rh);
    if (collisions->getNumberOfReports() == 0) {
        delete collisions;
        return NULL;
    }
    
    CollisionReport *closestCollision = NULL;
    double closestCollisionDistance = 0;
    
    for (int i = 0; i < collisions->getNumberOfReports(); i++) {
        CollisionReport *currentCollisionReport = collisions->getReport(i);
        currentCollisionReport->setPlatform(p);
        double diffX = _player->getX() - currentCollisionReport->getIntersectionX();
        double diffY = _player->getY() - currentCollisionReport->getIntersectionY();
        double distance = sqrt(pow(diffX, 2) + pow(diffY, 2));
        
        if (closestCollision) {
            if ((_extending && distance < closestCollisionDistance) || (!_extending && distance > closestCollisionDistance)) {
                closestCollision = currentCollisionReport;
                closestCollisionDistance = distance;
            }
        } else {
            closestCollision = currentCollisionReport;
            closestCollisionDistance = distance;
        }
    }
    
    closestCollision = new CollisionReport(closestCollision->getIntersectionX(), closestCollision->getIntersectionY(), closestCollision->getPlatform());
    
    delete collisions;
    return closestCollision;
}

//bool rectsOverlap(double x1, double y1, int w1, int h1, double x2, double y2, int w2, int h2) {
//    if (x1 > x2 - w1 && x1 < x2 + w2 &&   // aligned x
//        y1 > y2 - h1 && y1 < y2 + h2) {   // aligned y
//        return true;
//    }
//
//    return false;
//}

bool GrappleSeeker::seek(Level *level) {
    CollisionReportContainer container;
    
    for (int i = 0; i < level->getNumberOfPlatforms(); i++) {
        CollisionReport *newCollision = collide(level->getPlatform(i));
        
        if (newCollision) {
            container.addReport(newCollision->getIntersectionX(), newCollision->getIntersectionY());
            container.getReport(container.getNumberOfReports() - 1)->setPlatform(newCollision->getPlatform());
            
            delete newCollision;
        }
    }
    
    if (container.getNumberOfReports() == 0) {
        _x += _velocityX;
        _y += _velocityY;
        
        if (_extending) {
            _velocityX = SEEK_SPEED * cos(_angle);
            _velocityY = SEEK_SPEED * sin(_angle);
        } else {
            double diffX;
            double diffY;
            
            if (_numberOfPivots != 0) {
                diffX = _pivots[_numberOfPivots - 1].getX() - _x;
                diffY = _pivots[_numberOfPivots - 1].getY() - _y;
                
                double pivotDistance = sqrt(pow(diffX, 2) + pow(diffY, 2));
                if (pivotDistance <= sqrt(pow(_velocityX, 2) + pow(_velocityY, 2))) {
                    _player->createRope(_pivots[_numberOfPivots - 1].getX(), _pivots[_numberOfPivots - 1].getY());
                    return true;
                }
            } else {
                diffX = _player->getX() + _player->getWidth() / 2 - _x;
                diffY = _player->getY() + _player->getHeight() / 2 - _y;
            }
            
            double playerAngle = atan2(diffY, diffX);
            
            _velocityX = RETRACT_SPEED * cos(playerAngle);
            _velocityY = RETRACT_SPEED * sin(playerAngle);
        }
        
        if (_extending && getCurrentLength() > MAX_ROPE_LENGTH) {
            _extending = false;
        } else if (!_extending && getCurrentLength() <= sqrt(pow(_velocityX, 2) + pow(_velocityY, 2))) {
            return true;
        }
        
        wrapCorners(level);
        
        return false;
    }
    
    CollisionReport *closestCollision = NULL;
    double closestCollisionDistance = 0;
    
    for (int i = 0; i < container.getNumberOfReports(); i++) {
        CollisionReport *currentCollisionReport = container.getReport(i);
        double diffX = _player->getX() - currentCollisionReport->getIntersectionX();
        double diffY = _player->getY() - currentCollisionReport->getIntersectionY();
        double distance = sqrt(pow(diffX, 2) + pow(diffY, 2));
        
        if (closestCollision) {
            if ((_extending && distance < closestCollisionDistance) || (!_extending && distance > closestCollisionDistance)) {
                closestCollision = currentCollisionReport;
                closestCollisionDistance = distance;
            }
        } else {
            closestCollision = currentCollisionReport;
            closestCollisionDistance = distance;
        }
    }
    
    double x = closestCollision->getIntersectionX();
    double y = closestCollision->getIntersectionY();
    
    while (rectsOverlap(x - 1, y - 1, 2, 2, closestCollision->getPlatform()->getX(), closestCollision->getPlatform()->getY(), closestCollision->getPlatform()->getWidth(), closestCollision->getPlatform()->getHeight())) {
        x -= _velocityX / 10;
        y -= _velocityY / 10;
    }
    
//    if (closestCollision->getIntersectionX() == closestCollision->getPlatform()->getX()) {
//        _player->createRope(closestCollision->getIntersectionX() - 2, closestCollision->getIntersectionY());
//    } else if (closestCollision->getIntersectionX() == closestCollision->getPlatform()->getX() + closestCollision->getPlatform()->getWidth()) {
//        _player->createRope(closestCollision->getIntersectionX() + 1, closestCollision->getIntersectionY());
//    }
//
//    if (closestCollision->getIntersectionY() == closestCollision->getPlatform()->getY()) {
//        _player->createRope(closestCollision->getIntersectionX(), closestCollision->getIntersectionY() - 2);
//    } else if (closestCollision->getIntersectionY() == closestCollision->getPlatform()->getY() + closestCollision->getPlatform()->getHeight()) {
//        _player->createRope(closestCollision->getIntersectionX(), closestCollision->getIntersectionY() + 1);
//    }
    
    _player->createRope(x, y);
    
    if (_numberOfPivots > 0) {
        Pivot *playerRopePivots = _player->getRope()->getPivots();
        for (int i = 0; i < _numberOfPivots; i++) {
            playerRopePivots[i].setX(_pivots[i].getX());
            playerRopePivots[i].setY(_pivots[i].getY());
            playerRopePivots[i].setDrawX(_pivots[i].getDrawX());
            playerRopePivots[i].setDrawY(_pivots[i].getDrawY());
            playerRopePivots[i].setAttachAngle(_pivots[i].getAttachAngle());

            _player->getRope()->setNumberOfPivots(i + 1);
        }
    }
    
    return true;
}

int GrappleSeeker::wrapCorners(Level *level) {
    for (int i = 0; i < level->getNumberOfPlatforms(); i++) {
        double x1, y1, x2, y2;
        double rx, ry, rw, rh;
        
        if (_numberOfPivots == 0) {
            x1 = _x;
            y1 = _y;
        } else {
            x1 = _pivots[_numberOfPivots - 1].getX();
            y1 = _pivots[_numberOfPivots - 1].getY();
        }
        
        x2 = _player->getX() + _player->getWidth() / 2;
        y2 = _player->getY() + _player->getHeight() / 2;
        
        rx = level->getPlatform(i)->getX();
        ry = level->getPlatform(i)->getY();
        rw = level->getPlatform(i)->getWidth();
        rh = level->getPlatform(i)->getHeight();
        
        if (checkLineRectCollision(x1, y1, x2, y2, rx, ry, rw, rh)) {
            // collision
            bool left = false;
            bool right = false;
            bool top = false;
            bool bottom = false;
            
            bool playerLeft = false;
            bool playerRight = false;
            bool playerTop = false;
            bool playerBottom = false;
            
            bool platformLeft = false;
            bool platformRight = false;
            bool platformTop = false;
            bool platformBottom = false;
            
            bool movingLeft = false;
            bool movingRight = false;
            bool movingUp = false;
            bool movingDown = false;
            
            if (_player->getX() + _player->getWidth() - 1 < level->getPlatform(i)->getX()) {
                left = true;
            } else if (_player->getX() >= level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth()) {
                right = true;
            }
            
            if (_player->getY() + _player->getHeight() - 1 < level->getPlatform(i)->getY()) {
                top = true;
            } else if (_player->getY() >= level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight()) {
                bottom = true;
            }
            
            if (_x < _player->getX()) {
                playerLeft = true;
            } else if (_x > _player->getX() + _player->getWidth()) {
                playerRight = true;
            }
            
            if (_y < _player->getY()) {
                playerTop = true;
            } else if (_y > _player->getY() + _player->getHeight()) {
                playerBottom = true;
            }
            
            if (_x < rx) {
                platformLeft = true;
            } else if (_x > rx + rw) {
                platformRight = true;
            }
            
            if (_y < ry) {
                platformTop = true;
            } else if (_y > ry + rh) {
                platformBottom = true;
            }
            
            if (_player->getVelocityX() > 0) {
                movingRight = true;
            } else if (_player->getVelocityX() < 0) {
                movingLeft = true;
            }
            
            if (_player->getVelocityY() > 0) {
                movingDown = true;
            } else if (_player->getVelocityY() < 0) {
                movingUp = true;
            }
            
//            printf("player velocity x: %f, player velocity y: %f\n", _player->getVelocityX(), _player->getVelocityY());
//            printf("left: %d, right: %d, top: %d, bottom: %d, movingLeft: %d, movingRight: %d, movingUp: %d, movingDown: %d\n", left, right, top, bottom, movingLeft, movingRight, movingUp, movingDown);
//            printf("onLeft: %d, onRight: %d, onTop: %d, onBottom: %d\n", onLeft, onRight, onTop, onBottom);
            
            bool valuesSet = false;
            if ((left && !bottom && movingUp && playerRight && !playerTop && platformBottom) || (bottom && !left && movingRight && playerTop && !playerRight && platformLeft)) {    // left/~bottom and bottom/~left
                // pivot @ bottom left
                _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() - 2);
                _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight() + 1);
                _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() - 1);
                _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight());
                valuesSet = true;
            } else if ((top && !right && movingLeft && playerBottom && !playerLeft && platformRight) || (right && !top && movingDown && playerLeft && !playerBottom && platformTop)) {    // top/~right and right/~top
                // pivot @ top right
                _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth() + 1);
                _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() - 2);
                _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth());
                _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() - 1);
                valuesSet = true;
            } else if ((left && !top && movingDown && playerRight && !playerBottom && platformTop) || (top && !left && movingRight && playerBottom && !playerRight && platformLeft)) {   // left/~top and top/~left
                // pivot @ top left
                _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() - 2);
                _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() - 2);
                _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() - 1);
                _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() - 1);
                valuesSet = true;
            } else if ((right && !bottom && movingUp && playerLeft && !playerTop && platformBottom) || (bottom && !right && movingLeft && playerTop && !playerLeft && platformLeft)) {  // right/~bottom and bottom/~right
                // pivot @ bottom right
                _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth() + 1);
                _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight() + 1);
                _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth());
                _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight());
                valuesSet = true;
            } else if (top && movingDown) {
                if (playerLeft) {
                    // pivot @ top left
                    _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() - 2);
                    _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() - 2);
                    _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() - 1);
                    _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() - 1);
                    valuesSet = true;
                } else if (playerRight) {
                    // pivot @ top right
                    _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth() + 1);
                    _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() - 2);
                    _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth());
                    _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() - 1);
                    valuesSet = true;
                }
            } else if (bottom && movingUp) {
                if (playerLeft) {
                    // pivot @ bottom left
                    _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() - 2);
                    _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight() + 1);
                    _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() - 1);
                    _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight());
                    valuesSet = true;
                } else if (playerRight) {
                    // pivot @ bottom right
                    _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth() + 1);
                    _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight() + 1);
                    _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth());
                    _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight());
                    valuesSet = true;
                }
            } else if (left && movingRight) {
                if (playerTop) {
                    // pivot @ top left
                    _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() - 2);
                    _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() - 2);
                    _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() - 1);
                    _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() - 1);
                    valuesSet = true;
                } else if (playerBottom) {
                    // pivot @ bottom left
                    _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() - 2);
                    _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight() + 1);
                    _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() - 1);
                    _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight());
                    valuesSet = true;
                }
            } else if (right && movingLeft) {
                if (playerTop) {
                    // pivot @ top right
                    _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth() + 1);
                    _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() - 2);
                    _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth());
                    _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() - 1);
                    valuesSet = true;
                } else if (playerBottom) {
                    // pivot @ bottom right
                    _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth() + 1);
                    _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight() + 1);
                    _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth());
                    _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight());
                    valuesSet = true;
                }
            }
            
            if (valuesSet) {
                double diffX;
                double diffY;
                
                if (_numberOfPivots) {
                    diffX = _pivots[_numberOfPivots - 1].getX() - _pivots[_numberOfPivots].getX();
                    diffY = _pivots[_numberOfPivots - 1].getY() - _pivots[_numberOfPivots].getY();
                } else {
                    diffX = _x - _pivots[_numberOfPivots].getX();
                    diffY = _y - _pivots[_numberOfPivots].getY();
                }
                
                _pivots[_numberOfPivots].setAttachAngle(atan2(diffY, diffX));
                
                _numberOfPivots++;
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
    }
    
    return -1;
}

void GrappleSeeker::draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
    
    if (_numberOfPivots > 0) {
        SDL_RenderDrawLine(renderer, _x, _y, _pivots[0].getDrawX(), _pivots[0].getDrawY());
        for (int i = 1; i < _numberOfPivots; i++) {
            SDL_RenderDrawLine(renderer, _pivots[i - 1].getDrawX(), _pivots[i - 1].getDrawY(), _pivots[i].getDrawX(), _pivots[i].getDrawY());
        }
        SDL_RenderDrawLine(renderer, _pivots[_numberOfPivots - 1].getDrawX(), _pivots[_numberOfPivots - 1].getDrawY(), _player->getX() + _player->getWidth() / 2, _player->getY() + _player->getHeight() / 2);
    } else {
        SDL_RenderDrawLine(renderer, static_cast<int>(_player->getX() + _player->getWidth() / 2), static_cast<int>(_player->getY() + _player->getHeight() / 2), _x, _y);
    }
    
    // square where the hook is
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_Rect grappleRect = { static_cast<int>(_x) - GRAPPLE_RECT_HALF_WIDTH, static_cast<int>(_y) - GRAPPLE_RECT_HALF_WIDTH, GRAPPLE_RECT_HALF_WIDTH * 2, GRAPPLE_RECT_HALF_WIDTH * 2 };
    SDL_RenderFillRect(renderer, &grappleRect);
}

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

Rope::~Rope() {
    delete[] _pivots;
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

int Rope::collideCorners(Level *level) {
    double diffX;
    double diffY;

    for (int i = 0; i < level->getNumberOfPlatforms(); i++) {
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
        
        rx = level->getPlatform(i)->getX();
        ry = level->getPlatform(i)->getY();
        rw = level->getPlatform(i)->getWidth();
        rh = level->getPlatform(i)->getHeight();
        
        if (checkLineRectCollision(x1, y1, x2, y2, rx, ry, rw, rh)) {
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
                        _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() - 2);
                        _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight() + 1);
                        _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() - 1);
                        _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight());
                    } else if (direction == DOWN || direction == LEFT) {
                        // moving 'left'
                        _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth() + 1);
                        _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() - 2);
                        _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth());
                        _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() - 1);
                    }
                } else if ((_angle > M_PI_2) || (_angle > -M_PI_2 && _angle < 0)) {
                    // up/right and down/left
                    if (direction == DOWN || direction == RIGHT) {
                        // moving 'right'
                        _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() - 2);
                        _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() - 2);
                        _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() - 1);
                        _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() - 1);
                    } else if (direction == UP || direction == LEFT) {
                        // moving 'left'
                        _pivots[_numberOfPivots].setX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth() + 1);
                        _pivots[_numberOfPivots].setY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight() + 1);
                        _pivots[_numberOfPivots].setDrawX(level->getPlatform(i)->getX() + level->getPlatform(i)->getWidth());
                        _pivots[_numberOfPivots].setDrawY(level->getPlatform(i)->getY() + level->getPlatform(i)->getHeight());
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
                
//                printf("New X: %d, New Y: %d, New Attach Angle: %f\n", _pivots[_numberOfPivots].getX(), _pivots[_numberOfPivots].getY(), _pivots[_numberOfPivots].getAttachAngle());
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

void Rope::increaseSlack() {
    _ropeLength += SLACK_CHANGE_SPEED;
    if (_ropeLength > MAX_ROPE_LENGTH) {
        _ropeLength = MAX_ROPE_LENGTH;
    }
}

void Rope::decreaseSlack() {
    _ropeLength -= SLACK_CHANGE_SPEED;
    if (_ropeLength < MIN_ROPE_LENGTH) {
        _ropeLength = MIN_ROPE_LENGTH;
    }
}

Pivot *Rope::getPivots() {
    return _pivots;
}

void Rope::setNumberOfPivots(int x) {
    _numberOfPivots = x;
}

bool Rope::update(Level *level) {
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
    collideCorners(level);
    
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
    
    // square where the hook is
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_Rect grappleRect = { _grappleX - GRAPPLE_RECT_HALF_WIDTH, _grappleY - GRAPPLE_RECT_HALF_WIDTH, GRAPPLE_RECT_HALF_WIDTH * 2, GRAPPLE_RECT_HALF_WIDTH * 2 };
    SDL_RenderFillRect(renderer, &grappleRect);
}

// collision code from https://www.jeffreythompson.org/collision-detection/line-rect.php (modified)
CollisionReportContainer *getLineRectangleCollision(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh) {
    CollisionReportContainer *newContainer = new CollisionReportContainer();
    
    // check if the line has hit any of the rectangle's sides
    // uses the Line/Line function below
    CollisionReport *leftCollisionReport = getLineCollision(x1,y1,x2,y2, rx,ry,rx, ry+rh);
    if (leftCollisionReport) {
        newContainer->addReport(leftCollisionReport->getIntersectionX(), leftCollisionReport->getIntersectionY());
        delete leftCollisionReport;
    }
    
    CollisionReport *rightCollisionReport = getLineCollision(x1,y1,x2,y2, rx+rw,ry, rx+rw,ry+rh);
    if (rightCollisionReport) {
        newContainer->addReport(rightCollisionReport->getIntersectionX(), rightCollisionReport->getIntersectionY());
        delete rightCollisionReport;
    }
    
    CollisionReport *topCollisionReport = getLineCollision(x1,y1,x2,y2, rx,ry, rx+rw,ry);
    if (topCollisionReport) {
        newContainer->addReport(topCollisionReport->getIntersectionX(), topCollisionReport->getIntersectionY());
        delete topCollisionReport;
    }
    
    CollisionReport *bottomCollisionReport = getLineCollision(x1,y1,x2,y2, rx,ry+rh, rx+rw,ry+rh);
    if (bottomCollisionReport) {
        newContainer->addReport(bottomCollisionReport->getIntersectionX(), bottomCollisionReport->getIntersectionY());
        delete bottomCollisionReport;
    }

    return newContainer;
}


CollisionReport *getLineCollision(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    // calculate the direction of the lines
    float uA = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
    float uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));

    // if uA and uB are between 0-1, lines are colliding
    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
        float intersectionX = x1 + (uA * (x2-x1));
        float intersectionY = y1 + (uA * (y2-y1));
        
        CollisionReport *report = new CollisionReport();
        report->setIntersectionX(intersectionX);
        report->setIntersectionY(intersectionY);
        
        return report;
    }
    
    return NULL;
}

bool checkLineRectCollision(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh) {

  // check if the line has hit any of the rectangle's sides
  // uses the Line/Line function below
  bool left = checkLineCollision(x1,y1,x2,y2, rx,ry,rx, ry+rh);
  bool right = checkLineCollision(x1,y1,x2,y2, rx+rw,ry, rx+rw,ry+rh);
  bool top = checkLineCollision(x1,y1,x2,y2, rx,ry, rx+rw,ry);
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
