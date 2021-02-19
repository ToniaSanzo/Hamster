/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster UWindow.h
*/
#pragma once
#include "ULib.h"




class UWindow
{
public:
    // Initializes member variables
    UWindow();

    // Creates window, with the given name, dimensions, and window icon
    bool init(std::string, float, float, std::string);

    // Creates renderer from internal window
    SDL_Renderer *createRenderer();

    // Handles window events
    bool handleEvent(SDL_Event &);

    // Deallocate resources
    void free();

    // Window updater methods
    void center();

    // Getters
    SDL_Renderer *getRenderer();
    
    // Window focii
    bool hasMouseFocus();
    bool hasKeyboardFocus();
    bool isMinimized();

private:
    // Window data
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    SDL_Surface *mIcon;

    // Window focus
    bool mMouseFocus;
    bool mKeyboardFocus;
    bool mFullscreen;
    bool mMinimized;
};