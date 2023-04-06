#ifndef player_hpp
#define player_hpp

#include <SDL2/SDL.h>

#include "level.hpp"

const int PLAYER_WIDTH = 32;
const int PLAYER_HEIGHT = 32;

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Player {
public:
    Player();
    
    double getX();
    double getY();
    int getWidth();
    int getHeight();
    
    bool update(const Uint8 *keys, Platform *level, int numPlatforms);
    void draw(SDL_Renderer *renderer);
    
    int checkCollision(Platform *p);
    
private:
    double _x;
    double _y;
    int _width;
    int _height;
    
    double _velocityX;
    double _velocityY;
};

#endif
