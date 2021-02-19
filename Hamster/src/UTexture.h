/*
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/18/2020
* File: Hamster UTexture.h
*/
#pragma once
#include "ULib.h"

// Texture wrapper class
class UTexture {

public:
    // Initialize variables
    UTexture();

    // Deallocate memory
    ~UTexture();

    // Loads image at specified path
    bool loadFromFile(std::string path);

    // Creates image from font string
    bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

    // Deallocates texture
    void free();

    // Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    // Set blending
    void setBlendMode(SDL_BlendMode blending);

    // Set alpha modulation
    void setAlpha(Uint8 alpha);

    // Renders texture at given point
    void render(int x, int y, SDL_Rect *clip = nullptr, double angle = 0.0, SDL_Point *center = nullptr, SDL_RendererFlip = SDL_FLIP_NONE);

    // Initialize UTexture
    void initUTexture(SDL_Renderer *rend);
    void initFont(TTF_Font *font);

    // update the window dimensions
    void updateWindowScale(double sc);

    // Gets image dimensions
    int getWidth();
    int getHeight();
    double getScale();
    double getWindowScale();

    // Set image scale
    void updateScale(double sc);
private:
    // The actual hardware texture, and the games renderer
    SDL_Texture *mTexture;
    SDL_Renderer *mRenderer;
    TTF_Font *mFont;

    // Image dimensions
    int mWidth, mHeight;
    double mWindowScale;
    double mScale;
};