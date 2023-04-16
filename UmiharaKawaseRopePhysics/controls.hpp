#ifndef controls_hpp
#define controls_hpp

#ifdef __APPLE__
#include <SDL2/SDL.h>
#endif

#ifdef _WIN64
#include <SDL.h>
#undef main
#endif

#include <stdio.h>

enum InputState {
    NONE,
    PRESSED,
    HELD
};

class KeyboardLayout {
public:
    void updateKey(const Uint8 *keys, int key, int *keyState);
    void update(const Uint8 *keys);
    
    int getLeftState();
    int getRightState();
    int getUpState();
    int getDownState();
    
    void setLeft(int left);
    void setRight(int right);
    void setUp(int up);
    void setDown(int down);
    
    int getConfirmState();
    int getBackState();
    
    void setConfirm(int confirm);
    void setBack(int back);
    
    int getPauseState();
    
    void setPause(int pause);
    
    int getJumpState();
    int getGrappleState();
    int getAirBlastState();
    
    void setJump(int jump);
    void setGrapple(int grapple);
    void setAirBlast(int airBlast);
    
    int getNextEditorModeState();
    int getPreviousEditorModeState();
    int getPlayToggleState();
    
    void setNextEditorMode(int nextEditorMode);
    void setPreviousEditorMode(int previousEditorMode);
    void setPlayToggle(int playToggle);
    
private:
    int _left;
    int _right;
    int _up;
    int _down;
    int _leftState;
    int _rightState;
    int _upState;
    int _downState;
    
    int _confirm;
    int _back;
    int _confirmState;
    int _backState;
    
    int _pause;
    int _pauseState;
    
    int _jump;
    int _grapple;
    int _airBlast;
    int _jumpState;
    int _grappleState;
    int _airBlastState;
    
    int _nextEditorMode;
    int _previousEditorMode;
    int _playToggle;
    int _nextEditorModeState;
    int _previousEditorModeState;
    int _playToggleState;
};

class MouseState {
public:
    int getX();
    int getY();
    int getMovementX();
    int getMovementY();
    
    int getLeftClickState();
    int getRightClickState();
    int getMiddleClickState();
    
    int getScrollX();
    int getScrollY();
    
    void setX(int x);
    void setY(int y);
    void setMovementX(int dX);
    void setMovementY(int dY);
    
    void setLeftClickState(int state);
    void setRightClickState(int state);
    void setMiddleClickState(int state);
    
    void setScrollX(int scrollX);
    void setScrollY(int scrollY);
    
    void update();
    
private:
    int _x;
    int _y;
    int _dX;    // horizontal movement since last frame
    int _dY;    // vertical movement since last frame
    
    int _leftClickState;
    int _rightClickState;
    int _middleClickState;
    
    int _scrollX;
    int _scrollY;
};

#endif
