/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/19/2021
* File: Hamster UFont.cpp
*/
#include "UFont.h"




// Position sleep z's spawn
const UVector3 UFont::Z_SPAWN_POS = UVector3(380, 470, 0); 

// The SleepZ string to render
const std::string UFont::Z_STRING = "z";

// The countdown text is black
const SDL_Color UFont::BLACK_TEXT = SDL_Color{ 0, 0, 0, 0xFF };

// The game clock is orange
const SDL_Color UFont::ORANGE_TEXT = SDL_Color{ 0xFF, 0x95, 0, 0xFF };

// The total play time in seconds
const int UFont::PLAY_TIME_SECONDS = 3;/*68*/




// Default constructor
UFont::UFont() 
{
    mRenderer = nullptr;
    mSleepFont = nullptr;
    mMediumFont = nullptr;
    mCountdownFont = nullptr;
    mCountdownFlag = false;
    mPlayingStarted = false;
    mCurrentlyPlaying = false;
    mEnterGameEndState = false;
    mRerenderHighscoreText = false;
    mCountdownTime = 0.f;
    mCountdownText = 3;
    mPlayingTime = 0.f;
    mHighscore = 0;
    mPrevTime = 0;
    mLoops = 0;
    mPlayingTimeCount = PLAY_TIME_SECONDS;
    mCountdownAlpha = mGameClockAlpha = mLoopCountAlpha = 255;
    for (int i = 0; i < CLOCK_CHAR_COUNT; ++i)
    {
        mGameClockBuffer[i] = 0;
    }
}




// Initialize UFont class
bool UFont::init(SDL_Renderer *aRenderer)
{
    // Initialize the success flag
    bool success = true;
    mRenderer = aRenderer;

    // Initialize the sleep font
    mSleepFont = TTF_OpenFont("assets/font.ttf", 18);
    if (!mSleepFont)
    {
        printf("Failed to load sleep font, SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }

    // Initialize the countdown font
    mCountdownFont = TTF_OpenFont("assets/font.ttf", 450);
    if (!mCountdownFont)
    {
        printf("Failed to load sleep font, SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }
    else
    {
        // Initialize the countdown textures
        mCountdownTexture = UTexture();
        mCountdownTexture.initUTexture(mRenderer);
        mCountdownTexture.initFont(mCountdownFont);
        // Load the countdown text
        if (!mCountdownTexture.loadFromRenderedText("[NO VALUE]", BLACK_TEXT))
        {
            printf("Failed to load countdown texture!\n");
            success = false;
        }
        else
        {
            mCountdownTexture.setBlendMode(SDL_BLENDMODE_BLEND);
            mCountdownTexture.setAlpha(mCountdownAlpha);
        }
    }

    // Initialize the game clock font
    mMediumFont = TTF_OpenFont("assets/font.ttf", 92);
    if (!mCountdownFont)
    {
        printf("Failed to load sleep font, SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }
    else
    {
        // Initialize the game clock texture
        mGameClockTexture.initUTexture(mRenderer);
        mGameClockTexture.initFont(mMediumFont);
        // Load the game clock text
        if (!mGameClockTexture.loadFromRenderedText("[NO VALUE]", ORANGE_TEXT))
        {
            printf("Failed to load game clock texture!\n");
            success = false;
        }
        else
        {
            mGameClockTexture.setBlendMode(SDL_BLENDMODE_BLEND);
            mGameClockTexture.setAlpha(mGameClockAlpha);
        }

        // Initialize the loop count texture
        mLoopCountTexture.initUTexture(mRenderer);
        mLoopCountTexture.initFont(mMediumFont);
        // Load the loop count text
        if (!mLoopCountTexture.loadFromRenderedText("[NO VALUE]", BLACK_TEXT))
        {
            printf("Failed to load loop count texture!\n");
            success = false;
        }
        else
        {
            mLoopCountTexture.setBlendMode(SDL_BLENDMODE_BLEND);
            mLoopCountTexture.setAlpha(mLoopCountAlpha);
        }

        // Initialize the loop count header texture
        mLoopCountHeaderTexture.initUTexture(mRenderer);
        mLoopCountHeaderTexture.initFont(mMediumFont);
        // Load the loop count header text
        if (!mLoopCountHeaderTexture.loadFromRenderedText("Number of Loops", BLACK_TEXT))
        {
            printf("Failed to load loop count header texture!\n");
                success = false;
        }
        else
        {
            mLoopCountTexture.setBlendMode(SDL_BLENDMODE_BLEND);
            mLoopCountTexture.setAlpha(mLoopCountAlpha);
        }

        // Initialize the input header texture
        mInputHeaderTexture.initUTexture(mRenderer);
        mInputHeaderTexture.initFont(mMediumFont);
        // Load the input header text
        if (!mInputHeaderTexture.loadFromRenderedText("New Hi-Score! Enter name.", BLACK_TEXT))
        {
            printf("Failed to load input header!\n");
                success = false;
        }

        // Initialize the username input text
        mInputTextTexture.initUTexture(mRenderer);
        mInputTextTexture.initFont(mMediumFont);
        // Load the username input text
        if (!mInputTextTexture.loadFromRenderedText("_ _ _", BLACK_TEXT))
        {
            printf("Failed to load username input text!\n");
            success = false;
        }

        // Initialize the highscore texture
        mHighscoreTexture.initUTexture(mRenderer);
        mHighscoreTexture.initFont(mSleepFont);
        // Load the hgihscore text
        if (!mHighscoreTexture.loadFromRenderedText("[NON VALID]", BLACK_TEXT))
        {
            printf("Failed to load highscore texture!\n");
            success = false;
        }
    }

    mTimer.start();

    return success;
}




void UFont::handleEventNewHighscore(SDL_Event &e)
{
    // Special key input
    if (e.type == SDL_KEYDOWN)
    {
        // Handle backspace
        if (e.key.keysym.sym == SDLK_BACKSPACE && mInputText.length() > 0)
        {
            // lop off character
            mInputText.pop_back();
            mRerenderHighscoreText = true;
        }

        // Handle enter
        if (e.key.keysym.sym == SDLK_RETURN)
        {
            while (mInputText.length() < 3)
            {
                mInputText += ' ';
            }

            // The user entered their username
            mUsername = mInputText;
            mInputText = "";
            mEnterGameEndState = true;
        }
    }
    // Handle text input event
    else if (e.type == SDL_TEXTINPUT)
    {
        // Append the character if necessary
        if (mInputText.length() < 3)
        {
            mInputText += toupper(*(e.text.text));
            mRerenderHighscoreText = true;
        }
    }

}




// Update the SleepZ's
void UFont::update(const float &dt)
{
    // Update the SleepZ's
    bool cleanZs = false;
    for (SleepZ *&z : mSleepZs)
    {
        z->update(dt);
        if (!z->mLive)
        {
            cleanZs = true;
        }
    }
    // Clean up the dead SleepZs when necessary
    while (cleanZs)
    {
        std::vector<SleepZ*>::iterator beg, end;
        for (beg = mSleepZs.begin(), end = mSleepZs.end(); beg != end && (*beg)->mLive; ++beg);

        // Found a dead sleep z
        if (beg != end)
        {
            (*beg)->free();
            delete (*beg);
            mSleepZs.erase(beg);
        }
        // No dead sleep z's found
        else
        {
            cleanZs = false;
        }
    }

    // If we are still counting down to the start of the game
    if (mCountdownFlag)
    {
        mCountdownTime += dt;

        // If the countdown time is set to "Go!" than fade the countdown text out of the scene
        // and once the opacity is 0 set the mCountdownFlag to false to stop the countdown
        if (mCountdownText == 0)
        {
            mCountdownTexture.setAlpha(mCountdownAlpha);
            if (--mCountdownAlpha == 0)
            {
                mCountdownFlag = false;
            }
        }
        // If the countdown text has not changed to "Go!" yet, and it's time to change the countdown
        // text, update the countdown text and set the mPlayingStarted flag to true because the 
        // countdown is over
        else if (mCountdownText == 1 && mCountdownTime >= 3)
        {
            --mCountdownText;
            mCountdownTexture.loadFromRenderedText("Go!", BLACK_TEXT);
            mPlayingTime = 0.f;
            mPlayingTimeCount = PLAY_TIME_SECONDS;
            mPlayingStarted = true;
            mCurrentlyPlaying = true;
        }
        // If the countdown text has not changed to "1" yet, and it's time to change the countdown
        // text, update the countdown text
        else if (mCountdownText == 2 && mCountdownTime >= 2)
        {
            mCountdownTexture.loadFromRenderedText(std::to_string(--mCountdownText), BLACK_TEXT);
        }
        // If the countdown text has not changed to "1" yet, and it's time to change the countdown
        // text, update the countdown text
        else if (mCountdownText == 3 && mCountdownTime >= 1)
        {
            mCountdownTexture.loadFromRenderedText(std::to_string(--mCountdownText), BLACK_TEXT);
        }
    }

    // If we are currently in the play state start the countdown
    if (mCurrentlyPlaying)
    {
        mPlayingTime += dt;
        if ((PLAY_TIME_SECONDS - mPlayingTime) < mPlayingTimeCount)
        {
            mPlayingTimeCount = PLAY_TIME_SECONDS - mPlayingTime;
            
            // Once mPlayingTimeCount becomes 0, the game has finished
            if (mPlayingTimeCount <= 0)
            {
                mCurrentlyPlaying = false;
                mPlayingTime = 0;
            }
            sprintf_s(mGameClockBuffer, "%d:%.2d", mPlayingTimeCount / 60, mPlayingTimeCount % 60);
            mGameClockTexture.loadFromRenderedText(mGameClockBuffer, ORANGE_TEXT);
        }
    }

    // If the high score username needs a rerender
    if (mRerenderHighscoreText)
    {
        // mInputText is empty
        if (mInputText.length() == 0)
        {
            // Render space texture
            mInputTextTexture.loadFromRenderedText("_ _ _", BLACK_TEXT);
        }
        // Text has one character
        else if (mInputText.length() == 1)
        {
            mInputTextTexture.loadFromRenderedText((mInputText + " _ _"), BLACK_TEXT);
        }
        // Text has two characters
        else if (mInputText.length() == 2)
        {
            mInputTextTexture.loadFromRenderedText((mInputText + " _"), BLACK_TEXT);
        }
        // Text has three characters
        else
        {
            mInputTextTexture.loadFromRenderedText(mInputText, BLACK_TEXT);
        }
    }
}




// Render the sleep z's
void UFont::renderSleepZs()
{
    for (SleepZ *z : mSleepZs)
    {
        z->render();
    }
}




// Render the countdown text
void UFont::renderCountdown()
{
    mCountdownTexture.render((ULib::SCREEN_DIMENSIONS.x - mCountdownTexture.getWidth()) / 2, (ULib::SCREEN_DIMENSIONS.y - mCountdownTexture.getHeight()) / 2);
}




// Render the game clock
void UFont::renderGameClock()
{
    mGameClockTexture.render((ULib::SCREEN_DIMENSIONS.x - mGameClockTexture.getWidth()) / 2, (ULib::SCREEN_DIMENSIONS.y / 5.0) - (mGameClockTexture.getHeight() / 2.0));
}




// Render the number of loops of the last run
void UFont::renderLoopCount()
{
    mLoopCountHeaderTexture.render((ULib::SCREEN_DIMENSIONS.x - mLoopCountHeaderTexture.getWidth()) / 2, ((ULib::SCREEN_DIMENSIONS.y - mLoopCountTexture.getHeight()) / 2) - 35);
    mLoopCountTexture.render((ULib::SCREEN_DIMENSIONS.x - mLoopCountTexture.getWidth()) / 2, ((ULib::SCREEN_DIMENSIONS.y - mLoopCountTexture.getHeight()) / 2) + 35);
}



// Render the new high score text
void UFont::renderNewHighScore()
{
    // Render the new highscore screen
    mInputHeaderTexture.render((ULib::SCREEN_DIMENSIONS.x - mInputHeaderTexture.getWidth()) / 2, ((ULib::SCREEN_DIMENSIONS.y - mInputHeaderTexture.getHeight()) / 2) - 35);
    mInputTextTexture.render((ULib::SCREEN_DIMENSIONS.x - mInputTextTexture.getWidth()) / 2, ((ULib::SCREEN_DIMENSIONS.y - mInputTextTexture.getHeight()) / 2) + 35);
}




// Render the high score in the top right of the window
void UFont::renderHighscore()
{
    mHighscoreTexture.render(static_cast<int>(ULib::SCREEN_DIMENSIONS.x * ((9.0 / 12.0))), static_cast<int>(ULib::SCREEN_DIMENSIONS.y * ((1.0 / 25.0))));
}




// Will add a Z to the sleepZs collection at a steady pace
void UFont::addZ(UVector3 aHamsterPosition, bool aHamsterDirectionForward)
{
    // Modulate between 0 and 3.3 seconds
    Uint32 currTime = mTimer.getTicks() % 3300;

    // If the timer modulated once again randomly generate obstacles
    if (mPrevTime > currTime)
    {
        // Based on the direction the 
        // hamster is facing 
        if (aHamsterDirectionForward)
        {
            SleepZ *tmpZ = new SleepZ();
            aHamsterPosition.x += 60;
            aHamsterPosition.y += 20;
            tmpZ->init(mRenderer, mSleepFont, Z_STRING, aHamsterPosition);
            mSleepZs.push_back(tmpZ);
        }
        else
        {
            SleepZ *tmpZ = new SleepZ();
            aHamsterPosition.x -= 60;
            aHamsterPosition.y += 20;
            tmpZ->init(mRenderer, mSleepFont, Z_STRING, aHamsterPosition);
            mSleepZs.push_back(tmpZ);
        }
    }

    mPrevTime = currTime;
}




// Set the number of loops for this run
void UFont::setLoopCount(int aLoops)
{
    // Number of loops you'd like to render to the screen
    mLoops = aLoops;
    mLoopCountTexture.loadFromRenderedText(std::to_string(mLoops), BLACK_TEXT);
}




// Start the countdown
void UFont::startCountdown()
{
    mCountdownText = 3;
    mCountdownTime = 0.f;
    mCountdownAlpha = 255;
    mCountdownTexture.loadFromRenderedText(std::to_string(mCountdownText), BLACK_TEXT);
    mCountdownFlag = true;
}




// Get the current highscore
unsigned int UFont::getHighscore()
{
    return mHighscore;
}




// Get the current highscore username
std::string UFont::getHighscoreUsername()
{
    return mUsername;
}




// If the high score was passed set the new highscore
void UFont::setHighscore(unsigned int aHighScore)
{
    mHighscore = aHighScore;
    std::string tmpStr = std::to_string(mHighscore);
    tmpStr = "Hi-Score: " + tmpStr + " " + mUsername;
    mHighscoreTexture.loadFromRenderedText(tmpStr.c_str(), BLACK_TEXT);
}




// Set the current highscore username
void UFont::setHighscoreUsername(std::string aUsername)
{
    mUsername = aUsername;
    std::string tmpStr = std::to_string(mHighscore);
    tmpStr = "Hi-Score: " + tmpStr + " " + mUsername;
    mHighscoreTexture.loadFromRenderedText(tmpStr.c_str(), BLACK_TEXT);
}




// Deallocate the resources allocated for the UFont
void UFont::free()
{
    // Close dangling pointer
    if (mRenderer)
    {
        mRenderer = nullptr;
    }

    // Free the countdown texture
    mCountdownTexture.free();
    mGameClockTexture.free();
    mLoopCountTexture.free();
    mLoopCountHeaderTexture.free();
    mInputTextTexture.free();
    mInputHeaderTexture.free();

    // Delete the sleep z's
    for (SleepZ *z : mSleepZs)
    {
        z->free();
        delete z;
    }
    mSleepZs.clear();

    // Free fonts
    TTF_CloseFont(mSleepFont);
    TTF_CloseFont(mCountdownFont);
    mSleepFont = nullptr;
}




// How long the texture exists before being deleted
const float SleepZ::LIVE_TIME = 3.5;

// How far from the original x value the texture will be when the sin function is at a max value
const float SleepZ::SIN_WAVE_MAX_MIN_VAL = 23;

// Multiply the current mTime by this value to get 1.5 periods in 3.5 seconds
const float SleepZ::PERIOD_AMPLIFIER = 1.69;

// The color of the sleep z text should be white
const SDL_Color SleepZ::WHITE_TEXT = SDL_Color{ 0xFF, 0xFF, 0xFF, 0xFF };

// How fast the sleep z rises in the y-axis
const float SleepZ::RISE_SPEED = -23;




// Default constructor
SleepZ::SleepZ()
{
    mTexture = nullptr;
    mTime = 0;
    mLive = true;
    mPosition = UVector3(0, 0, 0);
}




// Initialize the SleepZ
bool SleepZ::init(SDL_Renderer *aRenderer, TTF_Font *aFont, const std::string &aRenderText, const UVector3 &aPosition)
{
    bool success = true;
    mLive = true;

    // Initialize the texture
    mTexture = new UTexture();
    mTexture->initUTexture(aRenderer);
    mTexture->initFont(aFont);

    // Initialize what the sleep text contains
    if (!mTexture->loadFromRenderedText(aRenderText, WHITE_TEXT))
    {
        printf("Failed to load SleepZ text!\n");
        success = false;
    }
    else
    {
        mTexture->setBlendMode(SDL_BLENDMODE_BLEND);
        mTexture->setAlpha(255);
    }

    // Set the position of the sleep z
    mPosition = aPosition;

    // Set the sleep z time initially to 0
    mTime = 0.f;

    // Return the success of this initialization
    return success;
}




// Render the sleep z
void SleepZ::render()
{
    mTexture->render((mPosition.x - (mTexture->getWidth() / 2.0)) + (sin(mTime * PERIOD_AMPLIFIER) * SIN_WAVE_MAX_MIN_VAL), mPosition.y - (mTexture->getHeight() / 2.0));
}




// Update the sleep z
void SleepZ::update(const float &dt)
{
    // If this sleep z is still live
    if (mTime < LIVE_TIME)
    {
        // Determine the opacity of the sleep z based on the time it has been alive, than set the opacity of the trick texture
        Uint8 opacity = static_cast<Uint8>(SDL_MAX_UINT8 * (1.0 - (mTime / LIVE_TIME)));
        mTexture->setAlpha(opacity);

        // Update the position of the SleepZ based on the time that's passed
        mTime += dt;

        mPosition.y += RISE_SPEED * dt;
    }
    else
    {
        mLive = false;
    }
}




// Free the texture
void SleepZ::free()
{
    mTexture->free();
    delete mTexture;
    mTexture = nullptr;
}