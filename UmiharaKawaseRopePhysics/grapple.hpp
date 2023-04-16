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

const int MIN_ROPE_LENGTH = 8;
const int MAX_ROPE_LENGTH = 150;

class Player;

class CollisionReport {
public:
    CollisionReport();
    CollisionReport(float x, float y, Platform *platform);
    
    float getIntersectionX();
    float getIntersectionY();
    
    void setIntersectionX(float x);
    void setIntersectionY(float y);
    
    Platform *getPlatform();
    void setPlatform(Platform *platform);
    
private:
    float _intersectionX;
    float _intersectionY;
    
    Platform *_platform;
};

class CollisionReportContainer {
public:
    CollisionReportContainer();
    ~CollisionReportContainer();
    
    void addReport(float x, float y);
    CollisionReport *getReport(int i);
    
    int getNumberOfReports();
    
private:
    CollisionReport *_reports;
    int _numberOfReports;
    int _reportsCapacity;
};

class Pivot {
public:
    int getX();
    int getY();
    int getDrawX();
    int getDrawY();
    
    void setX(int x);
    void setY(int y);
    void setDrawX(int x);
    void setDrawY(int y);
    
    void setAttachAngle(double attachAngle);
    double getAttachAngle();
    
private:
    int _x;
    int _y;
    int _drawX;
    int _drawY;
    
    double _attachAngle;
};

class GrappleSeeker {
public:
    GrappleSeeker(Player *player, double angle);
    ~GrappleSeeker();
    
    void addVelocityX(double x);
    void addVelocityY(double y);
    
    double getCurrentLength();
    
    CollisionReport *collide(Platform *platform);
    bool seek(Level *level);
    
    int wrapCorners(Level *level);
    
    void draw(SDL_Renderer *renderer);
    
private:
    Player *_player;    // seeker origin
    
    double _angle;
    bool _extending;
    
    double _x;
    double _y;
    double _velocityX;
    double _velocityY;
    
    Pivot *_pivots;
    int _numberOfPivots;
    int _pivotsCapacity;
};

class Rope {
public:
    Rope(Player *p, int gX, int gY);
    ~Rope();
    
    double getAccelerationX();
    double getAccelerationY();
    
    double getCurrentLength();
    double getCurrentAngle();
    int collideCorners(Level *level);
    
    void increaseSlack();
    void decreaseSlack();
    
    Pivot *getPivots();
    void setNumberOfPivots(int x);
    
    bool update(Level *level);
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
