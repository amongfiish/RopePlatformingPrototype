#include "controls.hpp"

void KeyboardLayout::updateKey(const Uint8 *keys, int key, int *keyState) {
    if (keys[key]) {
        if (*keyState == NONE) {
            *keyState = PRESSED;
        } else {
            *keyState = HELD;
        }
    } else {
        *keyState = NONE;
    }
}

void KeyboardLayout::update(const Uint8 *keys) {
    updateKey(keys, _left, &_leftState);
    updateKey(keys, _right, &_rightState);
    updateKey(keys, _up, &_upState);
    updateKey(keys, _down, &_downState);
    
    updateKey(keys, _confirm, &_confirmState);
    updateKey(keys, _back, &_backState);
    updateKey(keys, _pause, &_pauseState);
    
    updateKey(keys, _jump, &_jumpState);
    updateKey(keys, _grapple, &_grappleState);
    updateKey(keys, _airBlast, &_airBlastState);
}

int KeyboardLayout::getLeftState() {
    return _leftState;
}

int KeyboardLayout::getRightState() {
    return _rightState;
}

int KeyboardLayout::getUpState() {
    return _upState;
}

int KeyboardLayout::getDownState() {
    return _downState;
}

void KeyboardLayout::setLeft(int left) {
    _left = left;
}

void KeyboardLayout::setRight(int right) {
    _right = right;
}

void KeyboardLayout::setUp(int up) {
    _up = up;
}

void KeyboardLayout::setDown(int down) {
    _down = down;
}

int KeyboardLayout::getConfirmState() {
    return _confirmState;
}

int KeyboardLayout::getBackState() {
    return _backState;
}

int KeyboardLayout::getPauseState() {
    return _pauseState;
}

void KeyboardLayout::setConfirm(int confirm) {
    _confirm = confirm;
}

void KeyboardLayout::setBack(int back) {
    _back = back;
}

void KeyboardLayout::setPause(int pause) {
    _pause = pause;
}

int KeyboardLayout::getJumpState() {
    return _jumpState;
}

int KeyboardLayout::getGrappleState()
{
    return _grappleState;
}

int KeyboardLayout::getAirBlastState() {
    return _airBlastState;
}

void KeyboardLayout::setJump(int jump) {
    _jump = jump;
}

void KeyboardLayout::setGrapple(int grapple) {
    _grapple = grapple;
}

void KeyboardLayout::setAirBlast(int airBlast) {
    _airBlast = airBlast;
}
