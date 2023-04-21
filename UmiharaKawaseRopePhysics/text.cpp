#include "text.hpp"
using namespace std;

TextBox::TextBox() {
    _x = 0;
    _y = 0;
    _width = 0;
    _height = 0;
    
    _font = NULL;
    
    _previousText = "";
    _text = "";
    
    _renderedText = NULL;
}

TextBox::~TextBox() {
    if (_renderedText) {
        SDL_DestroyTexture(_renderedText);
    }
    
    if (_font) {
        TTF_CloseFont(_font);
    }
}

bool TextBox::initFont() {
    return initFont(TEXT_SIZE);
}

bool TextBox::initFont(int fontSize) {
    if (!_font) {
        _font = TTF_OpenFont("font.ttf", TEXT_SIZE);
        
        if (!_font) {
            printf("Couldn't open font. Error: %s\n", TTF_GetError());
            return false;
        }
    }
    
    return true;
}

void TextBox::setColor(int r, int g, int b, int a) {
    _color.r = r;
    _color.g = g;
    _color.b = b;
    _color.a = a;
}

void TextBox::setText(string text) {
    _previousText = _text;
    _text = text;
}

void TextBox::setX(int x) {
    _x = x;
}

void TextBox::setY(int y) {
    _y = y;
}

void TextBox::setWidth(int w) {
    _width = w;
}

void TextBox::setHeight(int h) {
    _height = h;
}

SDL_Color *TextBox::getColor() {
    return &_color;
}

string TextBox::getText() {
    return _text;
}

void TextBox::deleteLast() {
    if (_text.length() > 0) {
        _text.pop_back();
    }
}

int TextBox::getX() {
    return _x;
}

int TextBox::getY() {
    return _y;
}

int TextBox::getWidth() {
    return _width;
}

int TextBox::getHeight() {
    return _height;
}

void TextBox::detectWidth() {
    _width = _height * static_cast<int>(_text.length()) / 2;
}

void TextBox::draw(SDL_Renderer *renderer) {
    draw(renderer, _x, _y);
}

void TextBox::draw(SDL_Renderer *renderer, int x, int y) {
    initFont(TEXT_SIZE);
    
    if (_text != _previousText || !_renderedText) {
        if (_renderedText) {
            SDL_DestroyTexture(_renderedText);
        }
        
//        printf("_text: %s\n", _text.c_str());
        SDL_Surface *textSurface = TTF_RenderText_Solid(_font, _text.c_str(), _color);
        _renderedText = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);
    }
    
    SDL_Rect textRect = { x, y, _width, _height };
    SDL_RenderCopy(renderer, _renderedText, NULL, &textRect);
}

TextSelection::TextSelection() {
    _active = false;
    
    _scrollable = false;
    _drawBoundary = 3;
    
    _selector.setText(">");
    _selector.setHeight(32);
    _selector.detectWidth();
    _selector.setX(0);
    _selector.setY(0);
    _selector.setColor(0xFF, 0xFF, 0xFF, 0xFF);
    
    _selection = 0;
    
    _text = new TextBox[10];
    _textLength = 0;
    _textCapacity = 10;
    
    _fontSize = TEXT_SIZE;
    _textHeight = 32;
    _textSpacing = 5;
    
    _x = 0;
    _y = 0;
}

TextSelection::~TextSelection() {
    delete[] _text;
}

void TextSelection::setActive(bool active) {
    _active = active;
}

void TextSelection::setScrollable(bool scrollable) {
    _scrollable = scrollable;
}

void TextSelection::setItemsToDisplay(int n) {
    _drawBoundary = n;
}

int TextSelection::getSelection() {
    return _selection;
}

void TextSelection::addOption(string text, int r, int g, int b, int a) {
    _text[_textLength].setText(text);
    _text[_textLength].setColor(r, g, b, a);
    _text[_textLength].setX(_x);
    _text[_textLength].setY(_y + _textHeight * _textLength + _textSpacing * _textLength);
    _text[_textLength].setHeight(_textHeight);
    _text[_textLength].detectWidth();
    _text[_textLength].initFont(_fontSize);
    
    _textLength++;
    if (_textLength >= _textCapacity) {
        TextBox *newText = new TextBox[_textCapacity * 2];
        
        for (int i = 0; i < _textLength; i++) {
            newText[i].setText(_text[i].getText());
            
            SDL_Color *textColor = _text[i].getColor();
            newText[i].setColor(textColor->r, textColor->g, textColor->b, textColor->a);
            newText[i].setX(_text[i].getX());
            newText[i].setY(_text[i].getY());
            newText[i].setWidth(_text[i].getWidth());
            newText[i].setHeight(_text[i].getHeight());
        }
        
        delete _text;
        _text = newText;
        _textCapacity *= 2;
    }
}

void TextSelection::editOptionText(int i, string text) {
    _text[i].setText(text);
}

void TextSelection::clearOptions() {
    _textLength = 0;
}

void TextSelection::setPos(int x, int y) {
    _x = x;
    _y = y;
    
    for (int i = 0; i < _textLength; i++) {
        _text[i].setX(_x);
        _text[i].setY(_y + _textHeight * i + _textSpacing * i);
    }
    
    _selector.setX(x - _selector.getWidth() - _textSpacing);
    _selector.setY(y);
}

int TextSelection::update(KeyboardLayout *keys) {
    _selector.initFont();
    
    if (_active) {
        if (keys->getUpState() == PRESSED) {
            _selection--;
        } else if (keys->getDownState() == PRESSED) {
            _selection++;
        }
        
        if (_selection >= _textLength) {
            _selection = 0;
        } else if (_selection < 0) {
            _selection = _textLength - 1;
        }
        
        if (_scrollable) {
            _selector.setY(_y);
        } else {
            _selector.setY(_y + _selection * _textHeight + _selection * _textSpacing);
        }
        
        if (keys->getConfirmState() == PRESSED) {
            return _selection;
        }
    }
    
    return -1;
}

void TextSelection::draw(SDL_Renderer *renderer) {
    if (_scrollable) {
        for (int i = ((_selection - _drawBoundary < 0) ? 0 : _selection - _drawBoundary); i < ((_selection + _drawBoundary + 1 >= _textLength) ? _textLength : _selection + _drawBoundary + 1); i++) {
            _text[i].draw(renderer, _text[i].getX(), _text[i].getY() - _selection * _textHeight - _selection * _textSpacing);
        }
    } else {
        for (int i = 0; i < _textLength; i++) {
            _text[i].draw(renderer);
        }
    }
    
    _selector.draw(renderer);
}

TextInput::TextInput() {
    TextInput(0, 0, 0, 32);
}

TextInput::TextInput(int x, int y, int w, int h) {
    _x = x;
    _y = y;
    _width = w;
    _height = h;
    
    _text.setText("");
    
    _text.setHeight(_height);
    _text.detectWidth();
}

string TextInput::getText() {
    return _text.getText();
}

void TextInput::setX(int x) {
    _x = x;
}

void TextInput::setY(int y) {
    _y = y;
}

void TextInput::setWidth(int w) {
    _width = w;
}

void TextInput::setHeight(int h) {
    _height = h;
    _text.setHeight(h - 10);
    _text.detectWidth();
}

void TextInput::deleteLast() {
    _text.deleteLast();
}

void TextInput::setColor(int r, int g, int b, int a) {
    _text.setColor(r, g, b, a);
//    printf("R: %d, G: %d, B: %d, A: %d\n", _text.getColor()->r, _text.getColor()->g, _text.getColor()->b, _text.getColor()->a);
}

void TextInput::reset() {
    _text.setText("");
}

void TextInput::update(char pressedLetters[], int numPressedLetters) {
    for (int i = 0; i < numPressedLetters; i++) {
        _text.setText(_text.getText() + pressedLetters[i]);
    }
    
    _text.detectWidth();
}

void TextInput::draw(SDL_Renderer *renderer) {
    SDL_Rect backgroundRect = { _x - 10, _y - 10, _width + 20, _height + 20 };
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &backgroundRect);
    
    _text.draw(renderer, _x, _y);
//    printf("Error: %s\n", TTF_GetError());
}
