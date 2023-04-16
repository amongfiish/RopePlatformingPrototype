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
    
    updateKey(keys, _nextEditorMode, &_nextEditorModeState);
    updateKey(keys, _previousEditorMode, &_previousEditorModeState);
    updateKey(keys, _playToggle, &_playToggleState);
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

int KeyboardLayout::getNextEditorModeState() {
    return _nextEditorModeState;
}

int KeyboardLayout::getPreviousEditorModeState() {
    return _previousEditorModeState;
}

int KeyboardLayout::getPlayToggleState() {
    return _playToggleState;
}

void KeyboardLayout::setNextEditorMode(int nextEditorMode) {
    _nextEditorMode = nextEditorMode;
}

void KeyboardLayout::setPreviousEditorMode(int previousEditorMode) {
    _previousEditorMode = previousEditorMode;
}

void KeyboardLayout::setPlayToggle(int playToggle) {
    _playToggle = playToggle;
}

int MouseState::getX() {
    return _x;
}

int MouseState::getY() {
    return _y;
}

int MouseState::getMovementX() {
    return _dX;
}

int MouseState::getMovementY() {
    return _dY;
}

int MouseState::getLeftClickState() {
    return _leftClickState;
}

int MouseState::getRightClickState() {
    return _rightClickState;
}

int MouseState::getMiddleClickState() {
    return _middleClickState;
}

int MouseState::getScrollX() {
    return _scrollX;
}

int MouseState::getScrollY() {
    return _scrollY;
}

void MouseState::setX(int x) {
    _x = x;
}

void MouseState::setY(int y) {
    _y = y;
}

void MouseState::setMovementX(int dX) {
    _dX = dX;
}

void MouseState::setMovementY(int dY) {
    _dY = dY;
}

void MouseState::setLeftClickState(int state) {
    _leftClickState = state;
}

void MouseState::setRightClickState(int state) {
    _rightClickState = state;
}

void MouseState::setMiddleClickState(int state) {
    _middleClickState = state;
}

void MouseState::setScrollX(int scrollX) {
    _scrollX = scrollX;
}

void MouseState::setScrollY(int scrollY) {
    _scrollY = scrollY;
}

void MouseState::update() {
    _dX = 0;
    _dY = 0;
    
    SDL_GetMouseState(&_x, &_y);
    
    if (_leftClickState == PRESSED) {
        _leftClickState = HELD;
    }
    
    if (_rightClickState == PRESSED) {
        _leftClickState = HELD;
    }
    
    if (_middleClickState == PRESSED) {
        _middleClickState = HELD;
    }
}
