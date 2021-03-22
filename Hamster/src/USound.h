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

    // Play the soft step sfx
    void playSoftStep();

    // Toggle the music
    void toggleMusic();

    // Mute the sound
    void toggleMute();

    // Mute status
    bool isMuted() { return mute; }

    // Deallocate and destroy the sounds
    void free();
private:
    // The music, and sound assets used
    Mix_Music *mWheelMusic;
    Mix_Chunk *mSoftStep;

    bool mute;
};