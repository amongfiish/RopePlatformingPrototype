#ifndef controls_hpp
#define controls_hpp

#include <stdio.h>
#include <SDL.h>

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
};

#endif
