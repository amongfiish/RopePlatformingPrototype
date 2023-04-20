#ifndef text_hpp
#define text_hpp

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif

#ifdef _WIN64
#include <SDL.h>
#include <SDL_ttf.h>
#undef main
#endif

#include <stdio.h>
#include <string>

#include "controls.hpp"
using namespace std;

const int TEXT_SIZE = 128;

class TextBox {
public:
    TextBox();
    ~TextBox();
    
    bool initFont();
    bool initFont(int fontSize);
    
    void setColor(int r, int g, int b, int a);
    void setText(string text);
    
    void setX(int x);
    void setY(int y);
    void setWidth(int w);
    void setHeight(int h);
    
    SDL_Color *getColor();
    string getText();
    
    void deleteLast();
    
    int getX();
    int getY();
    int getWidth();
    int getHeight();
    
    void detectWidth();
    
    void draw(SDL_Renderer *renderer);
    void draw(SDL_Renderer *renderer, int x, int y);
    
private:
    string _previousText;
    string _text;
    
    SDL_Color _color;
    TTF_Font *_font;
    SDL_Texture *_renderedText;
    
    int _x;
    int _y;
    int _width;
    int _height;
};

class TextSelection {
public:
    TextSelection();
    ~TextSelection();
    
    void setActive(bool active);
    
    void setScrollable(bool scrollable);
    void setItemsToDisplay(int n);
    
    int getSelection();
    
    void addOption(string text, int r, int g, int b, int a);
    void clearOptions();
    
    void setPos(int x, int y);
    
    int update(KeyboardLayout *keys);
    void draw(SDL_Renderer *renderer);
    
private:
    bool _active;
    
    bool _scrollable;
    int _drawBoundary;
    
    TextBox _selector;
    int _selection;
    
    TextBox *_text;
    int _textLength;
    int _textCapacity;
    
    int _fontSize;
    int _textHeight;
    int _textSpacing;
    
    int _x;
    int _y;
};

class TextInput {
public:
    TextInput();
    TextInput(int x, int y, int w, int h);
    
    string getText();
    void deleteLast();
    
    void setX(int x);
    void setY(int y);
    void setWidth(int w);
    void setHeight(int h);
    
    void setColor(int r, int g, int b, int a);
    
    void reset();
    
    void update(char pressedLetters[], int numPressedLetters);
    void draw(SDL_Renderer *renderer);
    
private:
    TextBox _text;
    
    int _x;
    int _y;
    int _width;
    int _height;
};

#endif
