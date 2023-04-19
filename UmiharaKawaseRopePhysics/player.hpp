#ifndef player_hpp
#define player_hpp

#ifdef __APPLE__
#include <SDL2/SDL.h>
#endif

#ifdef _WIN64
#include <SDL.h>
#endif

#include "level.hpp"
#include "grapple.hpp"
#include "controls.hpp"

const double JUMP_VELOCITY = 4;
const double AIR_BLAST_VELOCITY_X = 2;
const double AIR_BLAST_VELOCITY_Y = 4;

const double GRAVITY = 0.2;
const double SWING_SLOWDOWN = 0.005;  // air resistance
const double NORMAL_FRICTION = 0.1;
const double ICE_FRICTION = 0.01;

const double AIR_ACCELERATION = 0.03;
const double NORMAL_ACCELERATION = 0.2;
const double ICE_ACCELERATION = 0.05;
const double MAX_GROUND_VELOCITY = 2;
const double MAX_AIR_VELOCITY = 2;

const double MAX_VELOCITY_X = 8;
const double MAX_VELOCITY_Y = 8;

const int LEFT_EYE_POS = 4;
const int RIGHT_EYE_POS = 18;
const int EYE_HEIGHT = 10;
const int EYE_WIDTH = 10;
const int PUPIL_WIDTH = 4;
const int LOOK_DISTANCE = 3;

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
    
    Platform *getGroundedPlatform();
    
    Rope *getRope();
    
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
    int _aim;
    int _facing;
    
    Platform *_groundedPlatform;
    
    bool _canAirBlast;
    
    bool _wasCollidingVertically;
    bool _wasCollidingHorizontally;
    
    GrappleSeeker *_grappleSeeker;
    Rope *_rope;
};

bool rectsOverlap(double x1, double y1, int w1, int h1, double x2, double y2, int w2, int h2);

#endif
