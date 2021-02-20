/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/19/2021
* File: Hamster UFont.cpp
*/
#include "UFont.h"




// Default constructor
UFont::UFont() 
{
    mRenderer = nullptr;
    mSleepFont = nullptr;
}




// Initialize UFont class
bool UFont::init(SDL_Renderer *aRenderer)
{
    // Initialize the success flag
    bool success = true;
    mRenderer = aRenderer;

    // Initialize the title texture
    mSleepFont = TTF_OpenFont("assets/font.ttf", 18);
    if (!mSleepFont)
    {
        printf("Failed to load font, SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }
}