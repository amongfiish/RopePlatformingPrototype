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
    ICE,
    LAVA
};
constexpr int NUMBER_OF_PLATFORM_TYPES = 4;
const string PLATFORM_TYPE_STRINGS[NUMBER_OF_PLATFORM_TYPES] = { "NORMAL", "METAL", "ICE", "LAVA" };

const SDL_Color NORMAL_COLOR = { 0xFF, 0xFF, 0xFF, 0xFF };
const SDL_Color METAL_COLOR = { 0x6B, 0x6B, 0x6B, 0xFF };
const SDL_Color ICE_COLOR = { 0x00, 0xA2, 0xFF, 0xFF };
const SDL_Color LAVA_COLOR = { 0xFF, 0x3c, 0x00, 0xFF };

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
    void draw(SDL_Renderer *renderer, double cameraX, double cameraY);
    
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
    
    bool collideEndX(int x, int y, int w, int h);
    
    int getEndX();
    int getEndY();
    void setEndPos(int x, int y);
    
    int getMaxX();
    int getMaxY();
    
    void correctLevel();
    
    int platformExists(int x, int y);
    
    void addPlatform(int x, int y, int w, int h, int type);
    void removePlatform(int i);
    void resetLevel();
    Platform *getPlatform(int i);
    
    int getNumberOfPlatforms();
    
    double getFastestTime();
    void setFastestTime(double fastestTime);
    
    void draw(SDL_Renderer *renderer);
    void draw(SDL_Renderer *renderer, double cameraX, double cameraY);
    
    void saveLevel(string filename);
    void loadLevel(string filename);
    
private:
    int _startX;
    int _startY;
    
    int _endX;
    int _endY;
    
    int _maxX;
    int _maxY;
    
    Platform *_platforms;
    int _numberOfPlatforms;
    int _platformsCapacity;
    
    double _fastestTime;
};

#endif
