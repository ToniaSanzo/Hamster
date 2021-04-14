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
#include "UTimer.h"
#include "UFont.h"
#include "USound.h"

class UFont;
class SleepZ
{
public:
    // Constructor
    SleepZ();

    // Initialize the SleepZ
    bool init(SDL_Renderer *, TTF_Font *, const std::string &, const UVector3 &);

    // Render the SleepZ
    void render();

    // Update the trick texture
    void update(const float &);

    // Free the texture
    void free();

    // Whether the SleepZ is currently live
    bool mLive;
private:
    // Class constants
    static const SDL_Color WHITE_TEXT;
    static const float LIVE_TIME;
    static const float SIN_WAVE_MAX_MIN_VAL;
    static const float PERIOD_AMPLIFIER;
    static const float RISE_SPEED;

    // Pointer to the texture
    UTexture *mTexture;

    // How long  sleep z has been created
    float mTime;

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
    bool init(SDL_Renderer *, USound *);

    // Used so the player can enter a highscore username is necessary
    void handleEventNewHighscore(SDL_Event &);

    // Update the fonts
    void update(const float &);

    // Render the sleep z's
    void renderSleepZs();

    // Render the countdown text
    void renderCountdown();

    // Render the game clock
    void renderGameClock();

    // Render the number of loops of the last run
    void renderLoopCount();

    // Render the new high score text
    void renderNewHighScore();

    // Render the high score in the top right of the window
    void renderHighscore();

    // Will add a Z to the sleepZs collection at a steady pace
    void addZ(UVector3, bool);

    // Set the number of loops for this run
    void setLoopCount(int);

    // Set the countdown text
    void startCountdown();

    // Return and reset the state of mPlayingStarted
    bool checkPlayingStarted()
    {
        if (mPlayingStarted)
        {
            mPlayingStarted = false;
            return true;
        }
        return false;
    }

    // Return the status of mCurrentlyPlaying
    bool checkCurrentlyPlaying() 
    { 
        return mCurrentlyPlaying;
    }

    // Return and reset the state of mEnterGameEndState
    bool checkEnterGameEndState()
    {
        if (mEnterGameEndState)
        {
            mEnterGameEndState = false;
            return true;
        }

        return false;
    }

    // Get the current highscore
    unsigned int getHighscore();

    // Get the current highscore username
    std::string getHighscoreUsername();

    // Set the current highscore
    void setHighscore(unsigned int);

    // Set the current highscores username
    void setHighscoreUsername(std::string);

    // Deallocate the resources allocated for the UFont
    void free();

private:
    // Class constants
    static const SDL_Color BLACK_TEXT;
    static const SDL_Color ORANGE_TEXT;
    static const UVector3 Z_SPAWN_POS;
    static const std::string Z_STRING;
    static const int PLAY_TIME_SECONDS;
    static const int CLOCK_CHAR_COUNT = 5;


    // Pointer to the game renderer
    SDL_Renderer *mRenderer;

    // UFont textures
    UTexture mCountdownTexture, mGameClockTexture, mLoopCountTexture, mLoopCountHeaderTexture, mInputTextTexture, mInputHeaderTexture, mHighscoreTexture;

    // UFont fonts
    TTF_Font *mSleepFont, *mMediumFont, *mCountdownFont;

    // If the countdown is happening, or the play state has started
    bool mCountdownFlag, mPlayingStarted, mCurrentlyPlaying;

    // The timer and variable used for steadly generating sleep z's, and counting 
    // down 
    UTimer mTimer;
    Uint32 mPrevTime;
    float mCountdownTime, mPlayingTime;
    int mPlayingTimeCount, mLoops;
    Uint8 mCountdownText, mCountdownAlpha, mGameClockAlpha, mLoopCountAlpha;

    // The highscore and the username associated with that highscore 
    unsigned int mHighscore;
    std::string mUsername;

    // Member variables used to determine and store the name of the high 
    // score user
    std::string mInputText;
    bool mRerenderHighscoreText, mEnterGameEndState;

    // Buffer used to hold the game clock text
    char mGameClockBuffer[CLOCK_CHAR_COUNT];

    // The collection of SleepZ's
    std::vector<SleepZ*>  mSleepZs;

    // USound reference
    USound *mSounds;
};