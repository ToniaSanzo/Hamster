/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster UGame.h
*/
#pragma once
#include "GHamster.h"
#include "ULib.h"
#include "UTexture.h"
#include "UWindow.h"
#include "UFont.h"
#include "USound.h"



class UGame
{
public:
    // Different game states
    enum class GameState
    {
        START,
        EXIT_SALOON,
        WALKING,
        WHEEL_STOPPED,
        WHEEL_PLAY_STARTING,
        WHEEL_PLAYING,
        GAME_ENDED,
        NEW_HIGHSCORE
    };

    // Initializes internals
    UGame();

    // Initialize the game objects
    bool init(SDL_Renderer *, UWindow *);

    // Updates the game world
    void update(const float &);

    // Handle's events
    bool handleEvent(SDL_Event &);

    // Draw game world
    void render();

    // Free the resources
    void close();

private:
    // This is the fade time of the Hamster title
    const static float FADE_TIME;

    // Information used to make saving data easier
    const static int SAVED_DATA_COUNT = 4;
    const static int HI_SCORE_DATA    = 0;
    const static int USERNAME_CHAR_1  = 1;
    const static int USERNAME_CHAR_2  = 2;
    const static int USERNAME_CHAR_3  = 3;

    // The current state of the game
    GameState mCurrState;

    // Game renderer, and window
    SDL_Renderer *mRenderer;
    UWindow *mWindow;

    // Environment textures
    UTexture mBackgroundTexture, mGlassCageTexture, mHouseForegroundTexture, mWheelTexture, mWheelArmTexture;

    // Current rotation angle of the hamster wheel
    double mDegree;

    // Variables used to render the title
    UTexture mTitleTexture;
    float mTitleFadeTime;

    // The games sound object
    USound mSounds;

    // The hamster
    GHamster mHamster;
    UFont mFonts;

    // Number of steps the hamster has made this playthrough
    int mStepCount;
};