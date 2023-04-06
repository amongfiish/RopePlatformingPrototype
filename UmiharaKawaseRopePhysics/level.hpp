#ifndef level_hpp
#define level_hpp

#include <SDL2/SDL.h>

class Platform {
public:
    Platform();
    Platform(int x, int y, int w, int h);
    
    int getX();
    int getY();
    int getWidth();
    int getHeight();
    
    void setX(int x);
    void setY(int y);
    void setWidth(int w);
    void setHeight(int h);
    
    void draw(SDL_Renderer *renderer);
    
private:
    int _x;
    int _y;
    int _width;
    int _height;
};

#endif
