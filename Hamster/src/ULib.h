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

#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#define MIN(a,b)  (((a) < (b)) ? (a) : (b))

struct ULib
{
    // Window information
    const static std::string TITLE;
    const static UVector3 SCREEN_DIMENSIONS;
};