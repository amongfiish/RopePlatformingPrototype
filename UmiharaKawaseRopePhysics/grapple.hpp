#ifndef grapple_hpp
#define grapple_hpp

#include "player.hpp"
#include "level.hpp"

enum CORNERS {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

const int MIN_ROPE_LENGTH = 0;
const int MAX_ROPE_LENGTH = 96;

class Player;

class Pivot {
public:
    int getX();
    int getY();
    
    void setX(int x);
    void setY(int y);
    
    void setAttachAngle(double attachAngle);
    double getAttachAngle();
    
private:
    int _x;
    int _y;
    
    double _attachAngle;
};

class Rope {
public:
    Rope(Player *p, int gX, int gY);
    
    double getAccelerationX();
    double getAccelerationY();
    
    double getCurrentLength();
    double getCurrentAngle();
    int collideCorners(Platform *level, int numPlatforms);
    
    bool update(Platform *level, int numPlatforms);
    void draw(SDL_Renderer *renderer);
    
private:
    Player *_player;
    
    int _grappleX;
    int _grappleY;
    
    Pivot *_pivots;
    int _numberOfPivots;
    int _pivotsCapacity;
    
    double _ropeLength;
    double _angle;
    double _stretch;
    
    double _previousAngle;
};

#endif
