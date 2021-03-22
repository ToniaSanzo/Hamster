/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/19/2021
* File: Hamster UFont.h
*/
#include "UTimer.h"




// Init UTimer
UTimer::UTimer() {
    // Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}




// Start the clock
void UTimer::start() {
    // Start the timer
    mStarted = true;

    // Unpause the timer
    mPaused = false;

    // Get the current clock time
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}




// Stop the clock
void UTimer::stop() {
    // Stop the timer
    mStarted = false;

    // Unpause the timer
    mPaused = false;

    // Clear tick variables
    mStartTicks = 0;
    mPausedTicks = 0;
}




// Pause the clock
void UTimer::pause() {
    // If the timer is running and isn't already paused
    if (mStarted && !mPaused) {
        mPaused = true;

        // Calculate the paused ticks
        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }
}




// Unpause the clock
void UTimer::unpause() {
    // aIf the timer is running and paused 
    if (mStarted && mPaused) {
        // Unpause the timer
        mPaused = false;
    }
}




// Get the timer's time
Uint32 UTimer::getTicks() {
    // The actual timer time
    Uint32 time = 0;

    // If the timer is running
    if (mStarted) {
        // If the timer is paused
        if (mPaused) {
            time = mPausedTicks;
        }
        else {
            // Returnthe current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}




// Return if the clock is running or paused
bool UTimer::isStarted() {
    // Timer is running and paused or unpaused
    return mStarted;
}




// Return if the clock is paused
bool UTimer::isPaused() {
    // Timer is running and paused
    return mPaused && mStarted;
}