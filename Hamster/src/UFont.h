/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/19/2021
* File: Hamster UFont.h
*/
#pragma once
#include "ULib.h"
#include "UTexture.h"
class UFont;
class SleepZ
{
public:
    // Whether the SleepZ is currently live
    bool live;

    // Constructor
    SleepZ();

    // Initialize the SleepZ
    bool init(SDL_Renderer *, TTF_Font *, const std::string &, const UVector3 &);

    // Render the SleepZ
    void render(const UVector3 &);

    // Update the trick texture
    void update(const double &);

    // Free the texture
    void free();

private:
    // Class constants
    static const double LIVE_TIME;
    static const int X_RAND_VEL;
    static const int Y_RAND_VEL;

    // Pointer to the texture
    UTexture *mTexture;

    // Values used to update the SleepZ
    UVector3 mVel;

    // Current position of the SleepZ
    UVector3 mPosition;
};




// Class used to render fonts
class UFont
{
public:
    // Default initialization
    UFont();

    // Initialize the font textures
    bool init(SDL_Renderer *);

    // Render game title
    void renderTitle();

    // Update the fonts
    void update(const float &);

    void addZ(const UVector3 &);

    // Deallocate the resources allocated for the UFont
    void free();
private:
    // Pointer to the game renderer
    SDL_Renderer *mRenderer;

    // UFont textures
    TTF_Font *mSleepFont;

    // The collection of SleepZ's
    std::vector<SleepZ*>  sleepZs;

};