/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster UGame.cpp
*/
#include "UGame.h"




// Default constructor
UGame::UGame()
{
    mRenderer          = nullptr;
    mWindow            = nullptr;

    mHamster = GHamster();
}




// Initializes and loads all the game objects
bool UGame::init(SDL_Renderer *aRenderer, UWindow *aWindow)
{
    // Initialize success flag
    bool success = true;

    // Set the renderer and window
    mRenderer = aRenderer;
    mWindow   = aWindow;

    if (mRenderer == nullptr || mWindow == nullptr)
    {
        printf("Attempted to initialize the UGame object with nullptr! Pass in valid pointers in the UGame.init() function\n");
        success = false;
    }
    else
    {
        // Initialize the environment textures
        mBackgroundTexture.initUTexture(mRenderer);
        if (!mBackgroundTexture.loadFromFile("assets/background.png"))
        {
            std::printf("Failed to load background texture!\n");
            success = false;
        }

        mGlassCageTexture.initUTexture(mRenderer);
        if (!mGlassCageTexture.loadFromFile("assets/glass_cage.png"))
        {
            std::printf("Failed to load glass cage texture!\n");
            success = false;
        }

        mWheelArmTexture.initUTexture(mRenderer);
        if (!mWheelArmTexture.loadFromFile("assets/wheel_arm.png"))
        {
            std::printf("Failed to load wheel arm texture!\n");
            success = false;
        }

        mWheelTexture.initUTexture(mRenderer);
        if (!mWheelTexture.loadFromFile("assets/wheel.png"))
        {
            std::printf("Failed to load wheel texture!\n");
            success = false;
        }

        mHouseForegroundTexture.initUTexture(mRenderer);
        if (!mHouseForegroundTexture.loadFromFile("assets/house_foreground.png"))
        {
            std::printf("Failed to load house foreground texture!\n");
            success = false;
        }

        if (!mHamster.init(mRenderer, "assets/hamster.png"))
        {
            std::printf("Failed to load hamster!\n");
            success = false;
        }
        
    }

    return success;
}




// Update the game world based on the time since the last update
void UGame::update(const float &dt)
{
    mHamster.update(dt);
}





// Handle all the events on the queue
bool UGame::handleEvent(SDL_Event &e)
{
    if (e.type == SDL_QUIT) { return true; }

    mHamster.handleEvent(e);

    return false;
}




// Draw the game world to the screen
void UGame::render()
{
    mBackgroundTexture.render(0, 0);
    mWheelTexture.render(728, 122);
    mWheelArmTexture.render(784, 281);
    mHamster.render();
    mHouseForegroundTexture.render(210, 352);
    mGlassCageTexture.render(109, 69);
}




// Free the game objects
void UGame::close()
{
    if (mRenderer)
    {
        mRenderer = nullptr;
    }

    if (mWindow)
    {
        mWindow = nullptr;
    }

    // Free the environment texture
    mBackgroundTexture.free();
    mWheelTexture.free();
    mWheelArmTexture.free();
    mHouseForegroundTexture.free();
    mGlassCageTexture.free();

    // Free the hamster
    mHamster.free();
    std::printf("Hector the Hamster has been liberated.\n");
}