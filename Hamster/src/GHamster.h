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
#include "UFont.h"
#include "USound.h"

// Animates dust being kicked up by the hamsters
class DustBall
{
public:
    // Whether this DustBall is currently live
    bool live;

    // Constructor
    DustBall();

    // Initialize the DustBall
    bool init(SDL_Renderer *, const UVector3 &);

    // Render the DustBall
    void render();

    // Update the DustBall
    void update(const float &);

    // Free the texture
    void free();

private:
    // Class constants
    static const float LIVE_TIME;
    static const int X_RAND_VEL;
    static const int Y_RAND_VEL;
    static const int DB_FRAME_WIDTH;
    static const int DB_FRAME_HEIGHT;
    static const int DB_FRAME_COUNT = 3;

    // The DustBall texture, frame rectangles, and current frame
    UTexture mTexture;
    SDL_Rect mDBAnimationFrames[DB_FRAME_COUNT];
    int mCurrFrame, mInitialFrame;

    // Values used to update the DustBall
    UVector3 mVel;
    float mTime;

    // Current position of the DustBall
    UVector3 mPosition;
};

// Hamster object
class GHamster
{
public:
    // Class constants
    const static int STEP_SIZE;
    const static int FRAME_WIDTH;
    const static int FRAME_HEIGHT;
    const static float WALK_BASE_POS;
    const static float WHEEL_BASE_POS;
    const static float RIGHT_WALL_POS;
    const static float LEFT_WALL_POS;
    const static float EXIT_SALOON_POS;
    const static float WHEEL_LEFT_POS;
    const static float WHEEL_RIGHT_POS;
    const static float CLIMB_UP_POS;
    const static float CLIMB_DOWN_POS;

    // Game states of the hamster
    enum class GameState
    {
        START,
        EXIT_SALOON,
        WALKING,
        WHEEL_STOPPED,
        WHEEL_PLAY_STARTING,
        WHEEL_PLAYING,
        GAME_ENDED,
        NEW_HIGHSCORE,
        SETTINGS_MENU
    };

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
    bool init(SDL_Renderer *, const std::string &, USound *);

    // Updates the hamster
    void update(const float &);

    // handles event
    void handleEvent(SDL_Event &);

    // Draw the hamster
    void render();

    // If the hamster is currently sleeping
    bool sleeping();

    // The current position of the hamster
    UVector3 getPosition() { return mPosition; }

    // If the hamsters direction is facing forward
    bool directionForward() { return mDirectionForward; }

    // The current hamster state
    int getState() { return static_cast<int>(mCurrState); }

    // Set the hamsters state
    void setState(int aState) { mCurrState = static_cast<GameState>(aState); }

    // Generate a new DustBall to the hamster
    void addDustBall();

    // Free the resources
    void free();

private:
    // The hamsters current state
    GameState mCurrState;

    // The hamsters position, and direction
    UVector3 mPosition;
    bool mDirectionForward;

    // Pointer to the sound object
    USound *mSound;

    // Pointer to the SDL_Renderer
    SDL_Renderer *mRenderer;

    // The hamsters sprite sheet, animation frames, current animation frame, and the amount of time the hamsters been in a certain frame
    UTexture mSpriteSheet;
    SDL_Rect mAnimationFrames[6];
    int mCurrFrame;
    float mFrameTime;

    // Collection of DustBalls
    std::vector<DustBall*> mDustBalls;
};