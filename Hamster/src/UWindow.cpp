/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster UWindow.cpp
*/

#include "UWindow.h"




// Initialize non-existant window
UWindow::UWindow()
{
    mIcon = nullptr;
    mWindow = nullptr;
    mRenderer = nullptr;
    mMouseFocus = mKeyboardFocus = mFullscreen = mMinimized = false;
}




// Create a non-resizeable window with the title and dimensions specified by the argument values
// 
// Returns true if mWindow is successfully initialized, otherwise false
bool UWindow::init(std::string title, float aW, float aH, std::string path = "")
{
    // Initialize the success flag
    bool success = true;

    mWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, aW, aH, SDL_WINDOW_SHOWN);

    if (mWindow != nullptr)
    {
        mMouseFocus = true;
        mKeyboardFocus = true;
        mRenderer = createRenderer();
    }
    else
    {
        // printf("Failed to create the SDL window!\n");
        success = false;
    }

    // Confirm mRenderer is initialized, and path is not the empty string
    if (mRenderer && !path.empty())
    {
        // Create the surtface
        mIcon = IMG_Load(path.c_str());
        if (!mIcon)
        {
            // printf("Unable to load image \"%s\"! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
            success = false;
        }
        else
        {
            // Set the window icon
            SDL_SetWindowIcon(mWindow, mIcon);
        }
    }

    return success;
}




// Creates a pointer to renderer associated with this window object
SDL_Renderer *UWindow::createRenderer() {
    return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}




// Handle the window events, i.e. KeyboardFocus, etc.
bool UWindow::handleEvent(SDL_Event &e) {
    // Window Eevent occured
    if (e.type == SDL_WINDOWEVENT) {
        switch (e.window.event) {
        // Repaint on exposure
        case SDL_WINDOWEVENT_EXPOSED:
            SDL_RenderPresent(mRenderer);
            break;

        // Mouse entered window
        case SDL_WINDOWEVENT_ENTER:
            mMouseFocus = true;
            break;

        // Mouse left window
        case SDL_WINDOWEVENT_LEAVE:
            mMouseFocus = false;
            break;

        // Window has keyboard focus
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            mKeyboardFocus = true;
            break;

        // Window lost keyboard focus
        case SDL_WINDOWEVENT_FOCUS_LOST:
            mKeyboardFocus = false;
            break;

        // Window minimized
        case SDL_WINDOWEVENT_MINIMIZED:
            mMinimized = true;
            break;

        // Window restored
        case SDL_WINDOWEVENT_RESTORED:
            mMinimized = false;
            break;
        }
    }
    return false;
}




// Get's the window's renderer
SDL_Renderer *UWindow::getRenderer() { return mRenderer; }




// Returns if the mouse is within the window
bool UWindow::hasMouseFocus() { return mMouseFocus; }




// Returns if the keyboard is focus
bool UWindow::hasKeyboardFocus() { return mMouseFocus; }




// Returns if the window is minimized
bool UWindow::isMinimized() { return mMinimized; }




// Destroy's the renderer and the window associated with this UWindow object
void UWindow::free()
{
    if (mIcon)
    {
        SDL_FreeSurface(mIcon);
        mIcon = nullptr;
    }

    if (mWindow)
    {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    if (mRenderer)
    {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
    }
}




// Center the window on the desktop
void UWindow::center()
{
    SDL_SetWindowPosition(mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}