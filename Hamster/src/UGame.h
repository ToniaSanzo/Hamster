/**
* Copyright 2021 Tonia Sanzo �
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster UGame.h
*/
#pragma once
#include "ULib.h"
#include "UWindow.h"
#include "UTexture.h"




class UGame
{
public:
    // Initializes internals
    UGame();

    // Initialize the game objects
    bool init(SDL_Renderer *, UWindow *);

    // Updates the game world
    void update(const double &);

    // Handle's events
    bool handleEvent(SDL_Event &);

    // Draw game world
    void render();

    // Free the resources
    void close();

private:
    // Game renderer, and window
    SDL_Renderer *mRenderer;
    UWindow *mWindow;

    // Environment textures
    UTexture mBackgroundTexture, mGlassCageTexture, mHouseForegroundTexture, mWheelTexture, mWheelArmTexture;
};