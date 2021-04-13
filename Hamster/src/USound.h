/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 3/13/2021
* File: Hamster USound.h
*/
#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <cstdio>

class USound {
public:
    // Initialize the USound member variables
    USound();

    // Initialize the USound object
    bool init();

    // Play the wheel music
    void playWheelMusic();

    // Play the menu music
    void playMenuMusic();

    // Play the soft step sfx
    void playSoftStep();

    // Toggle the music
    void toggleMusicMute();

    // Toggle the sound effects
    void toggleSFXMute();

    // SFX mute status
    bool isSFXMuted() 
    { 
        return mMuteSFX; 
    }

    // Music mute status
    bool isMusicMuted()
    {
        return mMuteMusic;
    }

    // Deallocate and destroy the sounds
    void free();
private:
    // The music, and sound assets used
    Mix_Music *mWheelMusic, *mMenuMusic;
    Mix_Chunk *mSoftStep;

    // If the music or sound effects are muted
    bool mMuteSFX, mMuteMusic;
};