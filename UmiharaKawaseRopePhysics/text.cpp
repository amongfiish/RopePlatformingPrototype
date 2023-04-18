#include "text.hpp"

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
    _font = TTF_OpenFont("font.ttf", TEXT_SIZE);
    
    if (!_font) {
        printf("Couldn't open font. Error: %s\n", TTF_GetError());
        return false;
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

void TextBox::draw(SDL_Renderer *renderer) {
    if (_text != _previousText) {
        if (_renderedText) {
            SDL_DestroyTexture(_renderedText);
        }
        
        SDL_Surface *textSurface = TTF_RenderText_Solid(_font, _text.c_str(), _color);
        _renderedText = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);
    }
    
    SDL_Rect textRect = { _x, _y, _width, _height };
    SDL_RenderCopy(renderer, _renderedText, NULL, &textRect);
}
