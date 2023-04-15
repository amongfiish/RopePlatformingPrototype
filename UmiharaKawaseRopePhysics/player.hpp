#ifndef player_hpp
#define player_hpp

#include <SDL2/SDL.h>

#include "level.hpp"
#include "grapple.hpp"
#include "controls.hpp"

const int PLAYER_WIDTH = 32;
const int PLAYER_HEIGHT = 32;

class Rope;
class GrappleSeeker;

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UPLEFT,
    UPRIGHT,
    DOWNLEFT,
    DOWNRIGHT
};

class Player {
public:
    Player();
    
    double getX();
    double getY();
    int getWidth();
    int getHeight();
    double getVelocityX();
    double getVelocityY();
    
    void setPos(int x, int y);
    void stop();
    
    void createGrappleSeeker(double angle);
    void destroyGrappleSeeker();
    
    void createRope(int gX, int gY);
    void destroyRope();
    
    bool update(KeyboardLayout *keys, Level *level);
    void draw(SDL_Renderer *renderer);
    
    int checkCollision(Platform *p);
    
private:
    double _x;
    double _y;
    int _width;
    int _height;
    
    double _velocityX;
    double _velocityY;
    
    bool _grounded;
    int _facing;
    
    bool _canAirBlast;
    
    GrappleSeeker *_grappleSeeker;
    Rope *_rope;
};

#endif
