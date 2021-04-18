/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster Main.cpp
*/
#include "ULib.h"
#include "UWindow.h"
#include "UGame.h"
#include "../../Steam/steam_api.h"

// Our custom window and renderer
UWindow gWindow;
SDL_Renderer *gRenderer = nullptr;

// initialize the SDL subsystems
bool init() 
{

    // Initialization flag
    bool success = true;

    // Checks if the game was launched through steam, and relaunches the game through steam
    // if it wasn't
    if (SteamAPI_RestartAppIfNecessary(1583410))
    {
        printf("(SteamAPI_RestartAppIfNecessary(1583410)) returned true.\n");
        success = false;
    }

    // Initializes the Steam API, sets up the global state and populates the interface 
    // pointers which are accessible via the global functions which match the name of the
    // interface. Must return successfully to 
    if (!SteamAPI_Init())
    {
        printf("Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed).\n");
        success = false;
    }

    // Initialize SDL subsystems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) 
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else 
    {
        // Create window
        if (!gWindow.init(ULib::TITLE, ULib::SCREEN_DIMENSIONS.x, ULib::SCREEN_DIMENSIONS.y, "assets/hamster_icon.png")) 
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else 
        {
            // Get the window's renderer
            gRenderer = gWindow.getRenderer();
            if (gRenderer == nullptr) 
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else 
            {
                // Set renderer color (WHITE)
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) 
                {
                    printf("SDL_image could not initialzie! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }

                // Initialize music
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) 
                {
                    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
                    success = false;
                }

                // Initialize fonts
                if (TTF_Init() == -1) 
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s \n", TTF_GetError());
                    success = false;
                }

            }
        }
    }
    return success;
}

// Closes and frees all the resources used to run the game
void close() 
{
    printf("Cleaning up...");

    // Destroy window
    gWindow.free();
    gRenderer = nullptr;

    // Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();

    // Free resources used by the steam API
    SteamAPI_Shutdown();

    printf(" done.\n");
}

// Main Method
int main(int argc, char *args[]) 
{
    // Start up SDL and create window
    if (!init()) 
    {
        printf("Failed to initialize!\n");
    }
    else 
    {
        // Our game wrapper class
        UGame game;
        if (!game.init(gRenderer, &gWindow)) 
        {
            printf("Failed to initialize UGame!\n");
        }
        else 
        {
            // Game running flag
            bool quit = false;
            bool windowScaleUpdate = false;

            // Event handler
            SDL_Event e;

            // Timing variables
            const int FPS = 60;
            const int frameDelay = 1000 / FPS;
            Uint32 fStart;
            int fTime;
            Uint32 pTime = SDL_GetTicks();
            Uint32 cTime = pTime;

            // dt(delta time) - time in seconds since the last update function was called
            float dt = 0.0;

            while (!quit) 
            {
                fStart = SDL_GetTicks();

                // Handle events
                while (SDL_PollEvent(&e) != 0)
                {
                    quit = game.handleEvent(e);
                }

                // Determine the amount of time in seconds since the last time update was called
                cTime = SDL_GetTicks();
                dt = static_cast<float>(cTime - pTime) / 1000.f;
                // Update game world
                game.update(dt);
                pTime = cTime;

                // Draw the game world to the screen
                SDL_SetRenderDrawColor(gRenderer, 0xD3, 0xD3, 0xD3, 0xFF);
                SDL_RenderClear(gRenderer);
                game.render();
                SDL_RenderPresent(gRenderer);

                // This measures how long this iteration of the loop took
                fTime = SDL_GetTicks() - fStart;

                // This keeps us from displaying more frames than 60
                if (frameDelay > fTime) 
                {
                    SDL_Delay(frameDelay - fTime);
                }
            }

            game.close();
        }
    }

    close();
    return 0;
}