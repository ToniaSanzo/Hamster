/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 3/23/2021
* File: Hamster GButton.h
*/
#pragma once
#include "ULib.h"
#include "USound.h"
#include "UTexture.h"
#include "UVector3.h"

// A interactive button within the game
class GButton
{
public:
    // Construct the GButton object
    GButton();

    // Initialize the GButton object
    bool init(SDL_Renderer *, const std::string &, const UVector3 &, const UVector3 &);

    // Draw the button
    void render(int);

    // Handle Events
    void handleEvent(SDL_Event &);

    // Checks if the button was clicked, and reset's the button
    bool clicked();

    // Free the resources allocated to the GButton
    void free();

private:

    // The sprite tile dimensions, position, texture, sound object, and click status of the button
    float mTileWidth, mTileHeight;
    SDL_Rect mTiles[2];
    UVector3 mPosition;
    UTexture mTexture;
    bool clickFlag;
};