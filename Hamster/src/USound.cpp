/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 3/13/2021
* File: Hamster USound.cpp
*/
#include "USound.h"




// Initialize USound member variables
USound::USound() {
    mWheelMusic = nullptr;
    mSoftStep = nullptr;
    mute = false;
}




// Initialize the USound object
bool USound::init() {
    bool success = true;

    // Load the wheel music
    mWheelMusic = Mix_LoadMUS("assets/wheel_mus.wav");
    if (mWheelMusic == nullptr) {
        printf("Failed to load wheel music! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    // Load the soft step sound effect
    mSoftStep = Mix_LoadWAV("assets/softstep.wav");
    if (mSoftStep == nullptr) {
        printf("Failed to load soft step sound! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    return success;
}




// Play wheel music
void USound::playWheelMusic() {
    // If there is music playing
    if (Mix_PlayingMusic() != 0) {
        // Stop the music
        Mix_HaltMusic();
    }

    // Play the wheel music
    Mix_PlayMusic(mWheelMusic, -1);

    // If it is currently muted
    if (mute) {
        // Pause the music
        Mix_PauseMusic();
    }
}




// Plays the soft step sfx
void USound::playSoftStep() {
    if (!mute)
    {
        Mix_PlayChannel(-1, mSoftStep, 0);
    }
}




// Toggle the music
void USound::toggleMusic() {
    // If the music is paused
    if (Mix_PausedMusic() == 1) {
        // Resume the music
        Mix_ResumeMusic();
    }
    // If the music is playing
    else {
        // Pause the music
        Mix_PauseMusic();
    }
}




// Mute the sound
void USound::toggleMute()
{
    if (mute)
    {
        mute = false;
    }

    else
    {
        mute = true;
    }
}




// Deallocate and destroy the sounds
void USound::free(){
    // Music is playing
    if (Mix_PlayingMusic() != 0) {
        // Stop the music
        Mix_HaltMusic();
    }

    // Free wheel music
    Mix_FreeMusic(mWheelMusic);
    mWheelMusic = nullptr;

    // Free the soft step sound effect
    Mix_FreeChunk(mSoftStep);
    mSoftStep = nullptr;
}