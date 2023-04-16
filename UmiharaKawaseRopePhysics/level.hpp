#ifndef level_hpp
#define level_hpp

#ifdef __APPLE__
#include <SDL2/SDL.h>
#endif

#ifdef _WIN64
#include <SDL.h>
#endif

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
    
    void setPos(int x, int y);
    
    void draw(SDL_Renderer *renderer);
    
private:
    int _x;
    int _y;
    int _width;
    int _height;
};

class Level {
public:
    Level();
    ~Level();
    
    void save();
    void load();
    
    int getStartX();
    int getStartY();
    void setStartPos(int x, int y);
    
    void addPlatform(int x, int y, int w, int h);
    void removePlatform(int i);
    Platform *getPlatform(int i);
    
    int getNumberOfPlatforms();
    
    void draw(SDL_Renderer *renderer);
    
private:
    int _startX;
    int _startY;
    
    Platform *_platforms;
    int _numberOfPlatforms;
    int _platformsCapacity;
};

#endif
