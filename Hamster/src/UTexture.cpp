/*
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/18/2020
* File: Hamster UTexture.cpp
*/
#include "UTexture.h"




// UTexture constructor
UTexture::UTexture() {
    // Initialize
    mTexture = NULL;
    mRenderer = NULL;
    mFont = NULL;
    mWidth = 0;
    mHeight = 0;
    mScale = 1;
    mWindowScale = 1;
}




// UTexture destructor
UTexture::~UTexture() {
    // Deallocate texture
    free();

    // Free renderer and font if necessary
    if (mRenderer) {
        mRenderer = NULL;
    }
    if (mFont) {
        mFont = NULL;
    }
}




// Load UTexture from a file
bool UTexture::loadFromFile(std::string path) {
    // Get rid of preexisting texture
    free();

    // Exit prematuraly if UTexture has not been properly initialized
    if (!mRenderer) {
        printf("Attempted to render a texture without initializing a renderer!\n");
        return false;
    }

    // The final texture
    SDL_Texture *newTexture = NULL;

    // Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        printf("unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else {
        // Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(mRenderer, loadedSurface);
        if (!newTexture) {
            printf("Unable to create texture from %s! SDL Error: %s\n:", path.c_str(), SDL_GetError());
        }
        else {
            // Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    // Return success
    mTexture = newTexture;
    return mTexture != NULL;
}




// Load a font texture
bool UTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
    // Get rid of preexisting texture
    free();

    // Exit prematuraly if UTexture has not been properly initialized
    if (!mFont) {
        printf("Attempted to render a text texture without initializing a font!\n");
        return false;
    }
    if (!mRenderer) {
        printf("Attempted to render a texture without initializing a renderer!\n");
        return false;
    }

    // Render text surface
    SDL_Surface *textSurface = TTF_RenderText_Blended(mFont, textureText.c_str(), textColor);
    if (textSurface == NULL) {
        printf("Unable to render text surface!  SDL_ttfError: %s\n", TTF_GetError());
    }
    else {
        // Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
        if (mTexture == NULL) {
            printf("Unable to create texture from rendered text! SDL_Error: %s\n", SDL_GetError());
        }
        else {
            // Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        // Get rid of old surface
        SDL_FreeSurface(textSurface);
    }

    // Return success
    return mTexture != NULL;
}




// Deallocate the texture
void UTexture::free() {
    // Free texture if it exists
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
        mScale = 0;
    }
}




// Define whether this texture can blend
void UTexture::setColor(Uint8 r, Uint8 g, Uint8 b) {
    // Modulate texture
    SDL_SetTextureColorMod(mTexture, r, g, b);
}




// Define whether this texture can blend
void UTexture::setBlendMode(SDL_BlendMode blending) {
    // Set blending function
    SDL_SetTextureBlendMode(mTexture, blending);
}




// Define the texture's opacity
void UTexture::setAlpha(Uint8 alpha) {
    // Modulate texture alpha
    SDL_SetTextureAlphaMod(mTexture, alpha);
}




// Render texture
void UTexture::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    // Set Rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth * mWindowScale, mHeight * mWindowScale };

    // Set clip rendering dimensions
    if (clip != NULL) {
        renderQuad.w = clip->w * mScale * mWindowScale;
        renderQuad.h = clip->h * mScale * mWindowScale;
    }

    // Render to screen
    SDL_RenderCopyEx(mRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}




// initialize UTexture with a renderer
void UTexture::initUTexture(SDL_Renderer *rend) {
    mRenderer = rend;
}




// Initialize the font
void UTexture::initFont(TTF_Font *font) {
    mFont = font;
}




// Getters
int UTexture::getHeight() { return mHeight; }
int UTexture::getWidth() { return mWidth; }
double UTexture::getScale() { return mScale; }
double UTexture::getWindowScale() { return mWindowScale; }




// Set image scale
void UTexture::updateScale(double sc) {
    mScale = sc;
    mWidth = static_cast<double>(mWidth) * sc;
    mHeight = static_cast<double>(mHeight) * sc;
}




// Update window scale
void UTexture::updateWindowScale(double sc)
{
    mWindowScale = sc;
}