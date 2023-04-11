#ifndef grapple_hpp
#define grapple_hpp

#include "player.hpp"

const int MIN_ROPE_LENGTH = 0;
const int MAX_ROPE_LENGTH = 96;

class Player;

class Rope {
public:
    Rope(Player *p, int gX, int gY);
    
    double getAccelerationX();
    double getAccelerationY();
    
    bool update();
    void draw(SDL_Renderer *renderer);
    
private:
    Player *_player;
    
    int _grappleX;
    int _grappleY;
    
    int **_pivots;
    
    double _ropeLength;
    double _angle;
    double _stretch;
};

#endif
