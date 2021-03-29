/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 3/23/2021
* File: Hamster GButton.h
*/
#include "GButton.h"

// Construct the GButton
GButton::GButton()
{
    mPosition = UVector3(0, 0, 0);
    clickFlag = false;
    mTileHeight = mTileWidth = 0;
    for (int i = 0; i < 2; ++i)
    {
        mTiles[i] = SDL_Rect();
    }
}

// Initialize the Gbutton
bool GButton::init(SDL_Renderer *aRenderer, const std::string &aPath, const UVector3 &aPosition, const UVector3 &aDimensions)
{
    // Initialize the success flag
    bool success = true;

    // Set the sprite tile dimensions
    mTileWidth = aDimensions.x;
    mTileHeight = aDimensions.y;

    // Initialize the button's texture
    mTexture = UTexture();
    mTexture.initUTexture(aRenderer);

    // Load the buttons texture from a file
    if (!mTexture.loadFromFile(aPath))
    {
        printf("Failed to load button texture!\n");
        success = false;
    }
    else
    {
        // Center buttons position around the argument's position value
        mPosition.x = aPosition.x - (mTileWidth / 2.0);
        mPosition.y = aPosition.y - (mTileHeight / 2.0);

        // Initialize the button tiles
        for (int i = 0; i < 2; ++i)
        {
            mTiles[i].x = i * mTileWidth;
            mTiles[i].y = 0;
            mTiles[i].w = mTileWidth;
            mTiles[i].h = mTileHeight;
        }
    }

    // Return the success of this initialization
    return success;
}

// Draw the button
void GButton::render(int aTileIndex)
{
    mTexture.render(mPosition.x, mPosition.y, &mTiles[aTileIndex]);
}

// Handle events
void GButton::handleEvent(SDL_Event &e)
{
    int mosX, mosY;

    // If the user pressed the mouse
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        // Get mouse position
        SDL_GetMouseState(&mosX, &mosY);

        // Check if mouse is in the button
        bool inside = true;

        // Mouse is left of the button
        if (mosX < (mPosition.x))
        {
            inside = false;
        }

        // Mouse is right of the button
        else if (mosX > (mPosition.x + (mTileWidth)))
        {
            inside = false;
        }

        // Mouse is above the button
        else if (mosY < mPosition.y)
        {
            inside = false;
        }

        // Mouse is below the button
        else if (mosY > mPosition.y + (mTileHeight))
        {
            inside = false;
        }

        // The button has been clicked
        if (inside)
        {
            clickFlag = true;
        }
    }
}

// Handle events
bool GButton::clicked()
{
    if (clickFlag) {
        clickFlag = false;
        return true;
    }
    else
    {
        return false;
    }
}

// Deallocate the resources allocated for this file
void GButton::free()
{
    mTexture.free();
}