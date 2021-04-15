/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster UGame.cpp
*/
#include "UGame.h"

// Seconds before the title fades out
const float UGame::FADE_TIME = 3;

// Dimensions and positions of the buttons
const UVector3 UGame::SETTINGS_BTN_DIMENSION   = UVector3{ 65, 65, 0 };
const UVector3 UGame::SETTINGS_BTN_POSITION    = UVector3{ 1220, 720, 0 };
const UVector3 UGame::MUSIC_BTN_POSITION       = UVector3{ 742, 232, 0 };
const UVector3 UGame::SFX_BTN_POSITION         = UVector3{ 976, 236, 0 };
const UVector3 UGame::SOUND_BTN_DIMENSION      = UVector3{ 53, 53, 0 };
const UVector3 UGame::PLAY_AGAIN_BTN_POSITION  = UVector3{ 644, 450, 0 };
const UVector3 UGame::PLAY_AGAIN_BTN_DIMENSION = UVector3{ 220, 85, 0 };

// Default constructor
UGame::UGame()
{
    mRenderer          = nullptr;
    mWindow            = nullptr;

    // Set the game state to start
    mCurrState = GameState::START;
    mPrevState = mCurrState;

    // The mTitleFadeTime is a variable to fade the title screen out
    mTitleFadeTime = 0;

    // Set the rotation angle of the hamster wheel
    mDegree = 0;

    // Set the number of steps the hamster has made on this playthrough
    mStepCount = 0;
}

// Initializes and loads all the game objects
bool UGame::init(SDL_Renderer *aRenderer, UWindow *aWindow)
{
    // Seed rand
    srand(static_cast<unsigned>(time(0)));

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
        {
            // Initialize the background texture
            mBackgroundTexture.initUTexture(mRenderer);
            if (!mBackgroundTexture.loadFromFile("assets/background.png"))
            {
                std::printf("Failed to load background texture!\n");
                success = false;
            }

            // Initialize the glass part of the cage texture
            mGlassCageTexture.initUTexture(mRenderer);
            if (!mGlassCageTexture.loadFromFile("assets/glass_cage.png"))
            {
                std::printf("Failed to load glass cage texture!\n");
                success = false;
            }

            // Initialize the texture of the arm that holds the wheel
            mWheelArmTexture.initUTexture(mRenderer);
            if (!mWheelArmTexture.loadFromFile("assets/wheel_arm.png"))
            {
                std::printf("Failed to load wheel arm texture!\n");
                success = false;
            }

            // Initialize the wheel texture
            mWheelTexture.initUTexture(mRenderer);
            if (!mWheelTexture.loadFromFile("assets/wheel.png"))
            {
                std::printf("Failed to load wheel texture!\n");
                success = false;
            }

            // Initialize the left side of the house texture
            mHouseForegroundTexture.initUTexture(mRenderer);
            if (!mHouseForegroundTexture.loadFromFile("assets/house_foreground.png"))
            {
                std::printf("Failed to load house foreground texture!\n");
                success = false;
            }

            // Initialize the title texture "Hamster"
            mTitleTexture.initUTexture(mRenderer);
            if (!mTitleTexture.loadFromFile("assets/title.png"))
            {
                std::printf("Failed to load title texture!\n");
                success = false;
            }
            else
            {
                mTitleTexture.setBlendMode(SDL_BLENDMODE_BLEND);
                mTitleTexture.setAlpha(255);
            }

            // Initialize the settings menu
            mSettingsMenuTexture.initUTexture(mRenderer);
            if (!mSettingsMenuTexture.loadFromFile("assets/settings_menu.png"))
            {
                std::printf("Failed to load settings menu texture!\n");
                success = false;
            }
        }

        // Initialize the sounds
        if (!mSounds.init())
        {
            printf("Failed to load sounds!\n");
            success = false;
        }
        else
        {
            mSounds.playMenuMusic();
        }

        // Initialize the hamster
        if (!mHamster.init(mRenderer, "assets/hamster.png", &mSounds))
        {
            std::printf("Failed to load hamster!\n");
            success = false;
        }

        // Initialize the fonts
        if (!mFonts.init(mRenderer, &mSounds))
        {
            std::printf("Failed to load fonts!\n");
            success = false;
        }

        // Initialize the settings button
        if (!mSettingsButton.init(mRenderer, "assets/settings_button.png", SETTINGS_BTN_POSITION, SETTINGS_BTN_DIMENSION))
        {
            std::printf("Failed to load the settings button!\n");
            success = false;
        }

        // Initialize the music button
        if (!mMusicButton.init(mRenderer, "assets/sound_button.png", MUSIC_BTN_POSITION, SOUND_BTN_DIMENSION))
        {
            std::printf("Failed to load the music button!\n");
            success = false;
        }

        // Initialize the sfx button
        if (!mSFXButton.init(mRenderer, "assets/sound_button.png", SFX_BTN_POSITION, SOUND_BTN_DIMENSION))
        {
            std::printf("Failed to load the sfx button!\n");
            success = false;
        }

        // Initialize the play again button
        if (!mPlayAgainButton.init(mRenderer, "assets/play_again_button.png", PLAY_AGAIN_BTN_POSITION, PLAY_AGAIN_BTN_DIMENSION))
        {
            std::printf("Failed to load the play again button!\n");
            success = false;
        }
    }

    // Attempt to read pre-saved data
    if (success)
    {
        // Open file for reading in binary
        SDL_RWops *file = SDL_RWFromFile("assets/save.bin", "r+b");

        // File does not exist
        if (file == nullptr)
        {
            printf("Warning: Unable to open file! SDL Error: %s\n", SDL_GetError());

            // Create file for writing
            file = SDL_RWFromFile("assets/save.bin", "w+b");

            if (file != nullptr)
            {
                // Initialize data
                long iData = 0;
                SDL_RWwrite(file, &iData, sizeof(long), 2);

                // Close file handler
                SDL_RWclose(file);

                // Set the high score to 0
                mFonts.setHighscore(0);
                mFonts.setHighscoreUsername("   ");
            }
            else
            {
                printf("Error: Unable to create file! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
        }

        // File exists
        else
        {
            // Default initialize the savedData array
            unsigned int savedData[SAVED_DATA_COUNT] = {};

            for (int i = 0; i < SAVED_DATA_COUNT; ++i)
            {
                SDL_RWread(file, &savedData[i], sizeof(long), 1);
            }

            // Read the highscore username from the saved data
            std::string usrnme;
            usrnme += static_cast<char>(savedData[USERNAME_CHAR_1]);
            usrnme += static_cast<char>(savedData[USERNAME_CHAR_2]);
            usrnme += static_cast<char>(savedData[USERNAME_CHAR_3]);

            // Set the highscore and highscore username from the saved data
            mFonts.setHighscore(savedData[HI_SCORE_DATA]);
            mFonts.setHighscoreUsername(usrnme);

            // Set the music and sound effects to the previous save state
            if (savedData[SFX_MUTED])
            {
                mSounds.toggleSFXMute();
            }
            if (savedData[MUSIC_MUTED])
            {
                mSounds. toggleMusicMute();
            }

            // Close file handler
            SDL_RWclose(file);
        }
    }

    return success;
}

// Update the game world based on the time since the last update
void UGame::update(const float &dt)
{
    // Check if the settings button has been clicked
    if (mSettingsButton.clicked())
    {
        // Close the settings menu
        if (mCurrState == GameState::SETTINGS_MENU)
        {
            mCurrState = mPrevState;
        }

        // Open the settings menu
        else
        {
            mPrevState = mCurrState;
            mCurrState = GameState::SETTINGS_MENU;
        }
    }

    // Add sleep Z's if the hamster is currently sleeping
    if (mHamster.sleeping() && mCurrState != GameState::SETTINGS_MENU)
    {
        mFonts.addZ(mHamster.getPosition(), mHamster.directionForward());
    }

    // Update the game based on the current state of the game
    switch (mCurrState)
    {
    // Update the sleep z's
    case GameState::START:
        // update the font's so that the sleep z's float through the air
        mFonts.update(dt);

        // If the hamster's state has been updated, update the game's game state to reflect this
        if(mHamster.getState() == static_cast<int>(GameState::EXIT_SALOON))
        {
            mCurrState = GameState::EXIT_SALOON;
        }

        break;

    // Update the sleep z's, the hamster, and fade the title out
    case GameState::EXIT_SALOON:
        // If the hamster's state has been updated, update the game's game state to reflect this
        if (mHamster.getState() == static_cast<int>(GameState::WALKING))
        {
            mCurrState = GameState::WALKING;
        }

    // Update the sleep z's, the hamster and fade the title out
    case GameState::WALKING:
        // Update the fonts so that any existing sleep z's do not freeze
        mFonts.update(dt);

        // Update the hamster so when idle the hamster will transition between [walking|climbing] > standing > sleeping
        mHamster.update(dt);

        // Fade the title screen out
        if (mTitleFadeTime < FADE_TIME)
        {
            mTitleTexture.setAlpha(static_cast<Uint8>(static_cast<float>(UINT8_MAX) - (mTitleFadeTime * (static_cast<float>(UINT8_MAX) / FADE_TIME))));
            mTitleFadeTime += dt;
        }
        else
        {
            mTitleTexture.setAlpha(0);
        }

        if(mHamster.getState() == static_cast<int>(GameState::WHEEL_STOPPED))
        {
            mCurrState = GameState::WHEEL_STOPPED;
        }

        break;

    // Update the states based on the state of the hamster
    case GameState::WHEEL_STOPPED:
        mFonts.update(dt);
        if (mHamster.getState() == static_cast<int>(GameState::WALKING))
        {
            mCurrState = GameState::WALKING;
        }
        else if (mHamster.getState() == static_cast<int>(GameState::WHEEL_PLAY_STARTING))
        {
            mSounds.playClack();
            mCurrState = GameState::WHEEL_PLAY_STARTING;
            mFonts.startCountdown();
        }
        break;

    // Update the mFonts so that the countdown will run and when it's
    // appropriate update the game state to the play state
    case GameState::WHEEL_PLAY_STARTING:
        // Update the fonts so that the countdown will run correctly
        mFonts.update(dt);

        // Update the game state when the countdown is over
        if (mFonts.checkPlayingStarted())
        {
            mCurrState = GameState::WHEEL_PLAYING;
            mHamster.setState(static_cast<int>(GameState::WHEEL_PLAYING));
            mStepCount = 0;
            mSounds.playWheelMusic();
        }
        break;

    // Update the mFonts so that the countdown will continue to fade out
    case GameState::WHEEL_PLAYING:
        mFonts.update(dt);
        mHamster.update(dt);
        
        // If the hamster is not currently playing change the state to the end state
        if (!mFonts.checkCurrentlyPlaying())
        {
            mFonts.setLoopCount(mStepCount / 5);

            // Enter new high score state
            if (mFonts.getHighscore() < mStepCount / 5)
            {
                mCurrState = GameState::NEW_HIGHSCORE;
                mHamster.setState(static_cast<int>(GameState::NEW_HIGHSCORE));
                mFonts.setHighscore(mStepCount / 5);
                mSounds.playMenuMusic();
            }

            // Enter game end state
            else
            {
                mCurrState = GameState::GAME_ENDED;
                mHamster.setState(static_cast<int>(GameState::GAME_ENDED));
                mSounds.playMenuMusic();
            }
        }
        break;

    // Update the hamster and mFonts so the screen doesn't freeze
    case GameState::GAME_ENDED:
        mFonts.update(dt);
        mHamster.update(dt);
        // Check to see if the user wants to play again
        if (mPlayAgainButton.clicked())
        {
            mCurrState = GameState::WHEEL_STOPPED;
            mHamster.setState(static_cast<int>(GameState::WHEEL_STOPPED));
        }
        break;

    // Update the hamster and mFonts so the screen doesn't freeze
    case GameState::NEW_HIGHSCORE:
        mFonts.update(dt);
        mHamster.update(dt);
        if (mFonts.checkEnterGameEndState())
        {
            mCurrState = GameState::GAME_ENDED;
            mHamster.setState(static_cast<int>(GameState::GAME_ENDED));
        }
        break;

    // Check to see if the music or sfx button have been clicked
    case GameState::SETTINGS_MENU:
        // Toggle sfx on click
        if (mSFXButton.clicked())
        {
            mSounds.toggleSFXMute();
        }

        // Toggle music on click
        if (mMusicButton.clicked())
        {
            mSounds.toggleMusicMute();
        }

    default:
        break;
    }
}

// Handle all the events on the queue
bool UGame::handleEvent(SDL_Event &e)
{
    if (e.type == SDL_QUIT) { return true; }

    if (mCurrState != GameState::SETTINGS_MENU)
    {
        mHamster.handleEvent(e);
    }
    mSettingsButton.handleEvent(e);

    if (mCurrState == GameState::WHEEL_PLAYING)
    {
        // If the any key was pressed
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
        {
            // Rotate the wheel of the hamster
            mDegree += 72;
            ++mStepCount;
            mHamster.addDustBall();
        }
    }
    else if (mCurrState == GameState::NEW_HIGHSCORE)
    {
        // Handle the user entering their username
        mFonts.handleEventNewHighscore(e);
    }
    else if (mCurrState == GameState::GAME_ENDED)
    {
        // Handle the user clicking the play again button
        mPlayAgainButton.handleEvent(e);
    }
    else if (mCurrState == GameState::SETTINGS_MENU)
    {
        // Handle the user clicking the sfx or music mute buttons
        mSFXButton.handleEvent(e);
        mMusicButton.handleEvent(e);
    }

    return false;
}

// Draw the game world to the screen
void UGame::render()
{
    // Used to render either the current or previous game state
    GameState gameState;

    // If the GameState is the settings menu render the previous game state
    if (mCurrState == GameState::SETTINGS_MENU)
    {
        gameState = mPrevState;
    }

    // Otherwise render the current game state
    else
    {
        gameState = mCurrState;
    }

    // Draw the game world based on the current state of the game
    switch (gameState)
    {
    case GameState::START:
        mBackgroundTexture.render(0, 0);
        mWheelTexture.render(728, 117);
        mWheelArmTexture.render(784, 281);
        mHamster.render();
        mHouseForegroundTexture.render(210, 352);
        mFonts.renderSleepZs();
        mGlassCageTexture.render(16, -11);
        mTitleTexture.render(static_cast<int>((ULib::SCREEN_DIMENSIONS.x / 2.0) - (mTitleTexture.getWidth() / 2.0) + 108.0), static_cast<int>((ULib::SCREEN_DIMENSIONS.y / 6.0) - 6.0));
        mFonts.renderHighscore();
        break;

    case GameState::EXIT_SALOON:
        mBackgroundTexture.render(0, 0);
        mWheelTexture.render(728, 117);
        mWheelArmTexture.render(784, 281);
        mHamster.render();
        mHouseForegroundTexture.render(210, 352);
        mFonts.renderSleepZs();
        mGlassCageTexture.render(16, -11);
        mTitleTexture.render(static_cast<int>((ULib::SCREEN_DIMENSIONS.x / 2.0) - (mTitleTexture.getWidth() / 2.0) + 108.0), static_cast<int>((ULib::SCREEN_DIMENSIONS.y / 6.0) - 6.0));
        mFonts.renderHighscore();
        break;

    case GameState::WALKING:
        mBackgroundTexture.render(0, 0);
        mWheelTexture.render(728, 117);
        mWheelArmTexture.render(784, 281);
        mHouseForegroundTexture.render(210, 352);
        mHamster.render();
        mFonts.renderSleepZs();
        mGlassCageTexture.render(16, -11);
        mTitleTexture.render(static_cast<int>((ULib::SCREEN_DIMENSIONS.x / 2.0) - (mTitleTexture.getWidth() / 2.0) + 108.0), static_cast<int>((ULib::SCREEN_DIMENSIONS.y / 6.0) - 6.0));
        mFonts.renderHighscore();
        break;

    case GameState::WHEEL_STOPPED:
        mBackgroundTexture.render(0, 0);
        mWheelTexture.render(728, 117);
        mHouseForegroundTexture.render(210, 352);
        mHamster.render();
        mFonts.renderSleepZs();
        mWheelArmTexture.render(784, 281);
        mGlassCageTexture.render(16, -11);
        mFonts.renderHighscore();
        break;

    case GameState::WHEEL_PLAY_STARTING:
        mBackgroundTexture.render(0, 0);
        mWheelTexture.render(728, 117);
        mHouseForegroundTexture.render(210, 352);
        mHamster.render();
        mFonts.renderSleepZs();
        mWheelArmTexture.render(784, 281);
        mGlassCageTexture.render(16, -11);
        mFonts.renderCountdown();
        mFonts.renderHighscore();
        break;

    case GameState::WHEEL_PLAYING:
        mBackgroundTexture.render(0, 0);
        mWheelTexture.render(728, 117, nullptr, mDegree);
        mHouseForegroundTexture.render(210, 352);
        mHamster.render();
        mFonts.renderSleepZs();
        mWheelArmTexture.render(784, 281);
        mGlassCageTexture.render(16, -11);
        mFonts.renderCountdown();
        mFonts.renderGameClock();
        mFonts.renderHighscore();
        break;

    case GameState::GAME_ENDED:
        mBackgroundTexture.render(0, 0);
        mWheelTexture.render(728, 117, nullptr, mDegree);
        mHouseForegroundTexture.render(210, 352);
        mHamster.render();
        mFonts.renderSleepZs();
        mWheelArmTexture.render(784, 281);
        mGlassCageTexture.render(16, -11);
        mFonts.renderLoopCount();
        mFonts.renderHighscore();
        mPlayAgainButton.render(0);
        break;
        
    case GameState::NEW_HIGHSCORE:
        mBackgroundTexture.render(0, 0);
        mWheelTexture.render(728, 117, nullptr, mDegree);
        mHouseForegroundTexture.render(210, 352);
        mHamster.render();
        mFonts.renderSleepZs();
        mWheelArmTexture.render(784, 281);
        mGlassCageTexture.render(16, -11);
        mFonts.renderNewHighScore();
        mFonts.renderHighscore();
        break;
    }

    // Render the settings menu
    if (mCurrState == GameState::SETTINGS_MENU)
    {
        mSettingsMenuTexture.render(0, 0);
        mMusicButton.render(mSounds.isMusicMuted());
        mSettingsButton.render(1);
        mSFXButton.render(mSounds.isSFXMuted());
    }

    // Otherwise just render the default settigns button
    else
    {
        mSettingsButton.render(0);
    }
}

// Free the game objects
void UGame::close()
{
    // Open data for writing
    SDL_RWops *file = SDL_RWFromFile("assets/save.bin", "w+b");
    if (file != nullptr)
    {
        // Initialize the sData array to contain the default value
        unsigned int sData[SAVED_DATA_COUNT] = {};

        // Get the current high score
        sData[HI_SCORE_DATA] = mFonts.getHighscore();

        // Get the high score username
        std::string usrnme = mFonts.getHighscoreUsername();
        sData[USERNAME_CHAR_1] = static_cast<unsigned int>(usrnme[0]);
        sData[USERNAME_CHAR_2] = static_cast<unsigned int>(usrnme[1]);
        sData[USERNAME_CHAR_3] = static_cast<unsigned int>(usrnme[2]);

        // Get the mute status of the SFX and the Music
        sData[SFX_MUTED]       = mSounds.isSFXMuted();
        sData[MUSIC_MUTED]     = mSounds.isMusicMuted();

        // Save the current window size, and the current high score
        for (int i = 0; i < SAVED_DATA_COUNT; ++i)
        {
            SDL_RWwrite(file, &sData[i], sizeof(unsigned int), 1);
        }

        // Close file handler
        SDL_RWclose(file);
    }
    else
    {
        printf("Error: Unable to save file! %s\n", SDL_GetError());
    }

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
    mTitleTexture.free();

    // Free the member variables
    mHamster.free();
    mFonts.free();
    mSettingsButton.free();
    mMusicButton.free();
    mSFXButton.free();
    mPlayAgainButton.free();
}