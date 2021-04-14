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
    mMenuMusic = nullptr;
    mClick = nullptr;
    mClack = nullptr;
    mSoftStep = nullptr;
    mMuteSFX = mMuteMusic = false;
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

    // Load the menu music
    mMenuMusic = Mix_LoadMUS("assets/menu_mus.wav");
    if (mMenuMusic == nullptr)
    {
        printf("Failed to load menu music! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    // Load the soft step sound effect
    mSoftStep = Mix_LoadWAV("assets/softstep.wav");
    if (mSoftStep == nullptr) {
        printf("Failed to load soft step sound! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    // Load the click sfx
    mClick = Mix_LoadWAV("assets/click.wav");
    if (mClick == nullptr)
    {
        printf("Failed to load click sound! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    // Load the clack sfx
    mClack = Mix_LoadWAV("assets/clack.wav");
    if (mClack == nullptr)
    {
        printf("Failed to load clack sound! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    return success;
}

// Play wheel music
void USound::playWheelMusic() 
{
    // If there is music playing
    if (Mix_PlayingMusic() != 0) {
        // Stop the music
        Mix_HaltMusic();
    }

    // Play the wheel music
    Mix_PlayMusic(mWheelMusic, -1);

    // If the music is currently muted
    if (mMuteMusic) {
        // Pause the music
        Mix_PauseMusic();
    }
}

// Play menu music
void USound::playMenuMusic()
{
    // If there is music playing
    if (Mix_PlayingMusic() != 0) {
        // Stop the music
        Mix_HaltMusic();
    }

    // Play the menu music
    Mix_PlayMusic(mMenuMusic, -1);

    // If the music is currently muted
    if (mMuteMusic) {
        // Pause the music
        Mix_PauseMusic();
    }
}

// Stop the music
void USound::stopMusic()
{
    Mix_HaltMusic();
}

// Plays the soft step sfx
void USound::playSoftStep() 
{
    if (!mMuteSFX)
    {
        Mix_PlayChannel(-1, mSoftStep, 0);
    }
}

// Play the click sfx
void USound::playClick()
{
    if (!mMuteSFX)
    {
        Mix_PlayChannel(-1, mClick, 0);
    }
}

// Play the clack sfx
void USound::playClack()
{
    if (!mMuteSFX)
    {
        Mix_PlayChannel(-1, mClack, 0);
    }
}

// Toggle the music
void USound::toggleMusicMute() {
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

    mMuteMusic = !mMuteMusic;
}

// Mute the sound
void USound::toggleSFXMute()
{
    mMuteSFX = !mMuteSFX;
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

    // Free menu music
    Mix_FreeMusic(mMenuMusic);
    mMenuMusic = nullptr;

    // Free the soft step sound effect
    Mix_FreeChunk(mSoftStep);
    mSoftStep = nullptr;

    // Free the click sound effect
    Mix_FreeChunk(mClick);
    mClick = nullptr;

    // Free the clack sound effect
    Mix_FreeChunk(mClack);
    mClack = nullptr;
}