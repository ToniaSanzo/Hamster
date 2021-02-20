/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster GHamster.cpp
*/
#include "GHamster.h"




// Class constants
const char GHamster::SLEEP_STATE = 0;
const char GHamster::WALK_STATE = 1;
const char GHamster::WHEEL_STATE = 2;
const char GHamster::CLIMBING_STATE = 3;
const int GHamster::FRAME_WIDTH = 170;
const int GHamster::FRAME_HEIGHT = 116;




// Initializes the hamster's member variables
GHamster::GHamster()
{
    mCurrState = SLEEP_STATE;
    mCurrFrame = static_cast<int>(FRAMES::SLEEPING);

    for (int i = 0; i < 5; ++i)
    {
        mAnimationFrames[i] = SDL_Rect();
    }
}




// Initialize the hamster object
bool GHamster::init(SDL_Renderer *aRenderer, const std::string &path)
{
    // Initialize the success flag
    bool success = true;

    // Initialize the hamster sprite sheet
    mSpriteSheet.initUTexture(aRenderer);

    // Load the sprite sheet with the file path
    if (!mSpriteSheet.loadFromFile(path))
    {
        printf("Failed to load hamster sprite sheet!\n");
        success = false;
    }
    else
    {
        mPosition = UVector3(320, 450, 0);

        // Initialize the animation tiles
        for (int row = 0; row < 3; ++row)
        {
            for (int col = 0; col < 2; ++col)
            {
                mAnimationFrames[(row * 2) + col].x = col * FRAME_WIDTH;
                mAnimationFrames[(row * 2) + col].y = row * FRAME_HEIGHT;
                mAnimationFrames[(row * 2) + col].w = FRAME_WIDTH;
                mAnimationFrames[(row * 2) + col].h = FRAME_HEIGHT;
            }
        }
    }

    return success;

}




// Updates the hamster
void GHamster::update(const float &dt)
{

}




// Handle the events
void GHamster::handleEvent(SDL_Event &e)
{
    // If a key was pressed
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_SPACE:
            printf("Hamster event space.\n");
            break;

        case SDLK_LEFT:
            printf("Hamster event left arrow.\n");
            break;

        case SDLK_RIGHT:
            printf("Hamster event right arrow.\n");
            break;
            
        }
    }
}




// Draw the hamster
void GHamster::render()
{
    mSpriteSheet.render(static_cast<int>(mPosition.x - (FRAME_WIDTH / 2.0)), static_cast<int>(mPosition.y - (FRAME_HEIGHT / 2.0)), &mAnimationFrames[mCurrFrame]);
}




// Deallocate the hamster's resources
void GHamster::free()
{
    mSpriteSheet.free();
}