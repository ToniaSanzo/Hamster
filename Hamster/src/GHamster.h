/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster GHamster.h
*/
#pragma once
#include "ULib.h"
#include "UTexture.h"




// Hamster object
class GHamster
{
public:
    // Class constants
    const static char SLEEP_STATE;
    const static char WALK_STATE;
    const static char WHEEL_STATE;
    const static char CLIMBING_STATE;
    const static int FRAME_WIDTH;
    const static int FRAME_HEIGHT;

    // Hamster frames
    enum class FRAMES
    {
        RIGHT_STEP,
        LEFT_STEP,
        STANDING,
        SLEEPING,
        CLIMBING
    };

    // Initializes the hamster's member variables
    GHamster();

    // Initilaize the hamster object
    bool init(SDL_Renderer *, const std::string &);

    // Updates the hamster
    void update(const float &);

    // handles event
    void handleEvent(SDL_Event &);

    // Draw the hamster
    void render();

    // Free the resources
    void free();

private:
    // The hamsters current state
    char mCurrState;

    // The hamsters position
    UVector3 mPosition;

    // The hamsters sprite sheet, animation frame, and current animation frame
    UTexture mSpriteSheet;
    SDL_Rect mAnimationFrames[6];
    int mCurrFrame;
};