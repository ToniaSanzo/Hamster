/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster ULib.h
*/
#pragma once
#include <cstdio>
#include <cmath>
#include <ctime>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
#include "UVector3.h"
#include <vector>

// Determine the max and min of two variables
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#define MIN(a,b)  (((a) < (b)) ? (a) : (b))

// Button Dimensions
#define OPTION_BTN_DIMENSION UVector3{65, 65, 0}
#define DIRECTION_BTN_POSITON_1 UVector3{817, 92, 0}
#define DIRECTION_BTN_POSITION_2 UVector3{472, 92, 0}

// Options for different leaderboard entries
enum ELeaderboardEntryOption
{
    k_ELoading = 0,
    k_EIOFail = 1,
    k_ENoScore = 2,
    k_EValid = 3,
    k_EInvalid = 4
};

struct ULib
{
    // Window information
    const static std::string TITLE;
    const static UVector3 SCREEN_DIMENSIONS;
};