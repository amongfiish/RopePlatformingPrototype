#ifndef text_hpp
#define text_hpp

#include <stdio.h>
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif

#ifdef _WIN64
#include <SDL.h>
#include <SDL_ttf.h>
#define main
#endif

#include <string>
using namespace std;

const int TEXT_SIZE = 32;

class TextBox {
public:
    TextBox();
    ~TextBox();
    
    void setColor(int r, int g, int b, int a);
    void setText(string text);
    
    void setX(int x);
    void setY(int y);
    void setWidth(int w);
    void setHeight(int h);
    
    void draw(SDL_Renderer *renderer);
    
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

#endif
