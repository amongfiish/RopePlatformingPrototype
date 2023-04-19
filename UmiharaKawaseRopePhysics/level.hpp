#ifndef level_hpp
#define level_hpp

#ifdef __APPLE__
#include <SDL2/SDL.h>
#endif

#ifdef _WIN64
#include <SDL.h>
#endif

#include <string>
using namespace std;

enum PlatformTypes {
    NORMAL,
    METAL,
    ICE
};
constexpr int NUMBER_OF_PLATFORM_TYPES = 3;
const string PLATFORM_TYPE_STRINGS[NUMBER_OF_PLATFORM_TYPES] = { "NORMAL", "METAL", "ICE" };

const int PLATFORM_WIDTH = 32;
const int PLATFORM_HEIGHT = 32;
const int MAP_WIDTH = 25;
const int MAP_HEIGHT = 15;

class Platform {
public:
    Platform();
    Platform(int x, int y, int w, int h, int type);
    
    int getX();
    int getY();
    int getWidth();
    int getHeight();
    
    void setX(int x);
    void setY(int y);
    void setWidth(int w);
    void setHeight(int h);
    
    void setPos(int x, int y);
    
    void setType(int type);
    int getType();
    
    void draw(SDL_Renderer *renderer);
    
private:
    int _x;
    int _y;
    int _width;
    int _height;
    
    int _type;
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
    
    int platformExists(int x, int y);
    
    void addPlatform(int x, int y, int w, int h);
    void removePlatform(int i);
    Platform *getPlatform(int i);
    
    int getNumberOfPlatforms();
    
    void draw(SDL_Renderer *renderer);
    
    void saveLevel(string filename);
    void loadLevel(string filename);
    
private:
    int _startX;
    int _startY;
    
    Platform *_platforms;
    int _numberOfPlatforms;
    int _platformsCapacity;
};

#endif
