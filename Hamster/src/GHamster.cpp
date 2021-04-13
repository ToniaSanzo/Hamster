/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 2/17/2021
* File: Hamster GHamster.cpp
*/
#include "GHamster.h"

// Class constants
const int GHamster::STEP_SIZE = 20;
const int GHamster::FRAME_WIDTH = 170;
const int GHamster::FRAME_HEIGHT = 116;
const float GHamster::WALK_BASE_POS = 466.f;
const float GHamster::WHEEL_BASE_POS = 406.f;
const float GHamster::RIGHT_WALL_POS = 1100.f;
const float GHamster::LEFT_WALL_POS = 200.f;
const float GHamster::EXIT_SALOON_POS = 400.f;
const float GHamster::WHEEL_LEFT_POS = 860.f;
const float GHamster::WHEEL_RIGHT_POS = 940.f;
const float GHamster::CLIMB_UP_POS = 898;
const float GHamster::CLIMB_DOWN_POS = 900;

// Initializes the hamster's member variables
GHamster::GHamster()
{
    mRenderer = nullptr;
    mCurrState = GameState::START;
    mCurrFrame = static_cast<int>(FRAMES::SLEEPING);
    mFrameTime = 0.f;

    // Set the direction of the hamster to forward
    mDirectionForward = true;
   
    // Set sound pointer to null
    mSound = nullptr;

    for (int i = 0; i < 6; ++i)
    {
        mAnimationFrames[i] = SDL_Rect();
    }
}

// Initialize the hamster object
bool GHamster::init(SDL_Renderer *aRenderer, const std::string &path, USound *aSound)
{
    // Initialize the success flag
    bool success = true;

    // Initialize the hamster sprite sheet
    mSpriteSheet.initUTexture(aRenderer);

    // Load the sprite sheet with the file path
    if (!mSpriteSheet.loadFromFile(path))
    {
        printf("Failed to load hamster sprite sheet!\n");
        success = false;
    }
    else
    {
        mRenderer = aRenderer;
        mPosition = UVector3(320, 450, 0);

        // Initialize the animation tiles
        for (int row = 0; row < 3; ++row)
        {
            for (int col = 0; col < 2; ++col)
            {
                mAnimationFrames[(row * 2) + col].x = col * FRAME_WIDTH;
                mAnimationFrames[(row * 2) + col].y = row * FRAME_HEIGHT;
                mAnimationFrames[(row * 2) + col].w = FRAME_WIDTH;
                mAnimationFrames[(row * 2) + col].h = FRAME_HEIGHT;
            }
        }
    }

    // Initialize the hamster's sound pointer
    mSound = aSound;
    if (mSound == nullptr)
    {
        printf("Failed to load passed in with invalid USound* argument!\n");
        success = false;
    }

    return success;
}

// Updates the hamster
void GHamster::update(const float &dt)
{
    // Add delta time to the current frame time
    mFrameTime += dt;

    // If the hamster is currently taking a step, and the current frame time is greater than 3 seconds switch to standing
    if ((mCurrFrame == static_cast<int>(FRAMES::RIGHT_STEP) || mCurrFrame == static_cast<int>(FRAMES::LEFT_STEP) || mCurrFrame == static_cast<int>(FRAMES::CLIMBING)) && mFrameTime >= 1.2f)
    {
        mCurrFrame = static_cast<int>(FRAMES::STANDING);
        mFrameTime = 0;
    }
    // If the hamster has been standing for 15 seconds fall asleep
    else if (mCurrFrame == static_cast<int>(FRAMES::STANDING) && mFrameTime >= 15.f && (mCurrState != GameState::WHEEL_PLAYING))
    {
        mCurrFrame = static_cast<int>(FRAMES::SLEEPING);
        mFrameTime = 0;
    }

    // Update the DustBalls
    bool cleanDBalls = false;
    for (DustBall *&tmpDustBall : mDustBalls)
    {
        tmpDustBall->update(dt);
        if (!tmpDustBall->live)
        {
            cleanDBalls = true;
        }
    }
    // Clean up the dead DustBalls when necessary
    while (cleanDBalls)
    {
        std::vector<DustBall*>::iterator beg, end;
        // Move the beg iterator to either the first dead DustBall or one past the last element in the mDustBalls collection
        for (beg = mDustBalls.begin(), end = mDustBalls.end(); beg != end && (*beg)->live; ++beg);

        // Found a dead mDustBalls
        if (beg != end)
        {
            (*beg)->free();
            mDustBalls.erase(beg);
        }
        // No dead DustBall was found
        else
        {
            cleanDBalls = false;
        }
    }
}

// Handle the events
void GHamster::handleEvent(SDL_Event &e)
{
    switch (mCurrState)
    {
    case GameState::START:
        // If the space key was pressed
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_SPACE)
        {
            mCurrState = GameState::EXIT_SALOON;
            mCurrFrame = static_cast<int>(FRAMES::STANDING);
            mFrameTime = 0;
        }

        break;
    case GameState::EXIT_SALOON:
        // If the space key was pressed
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_SPACE)
        {
            // Alternate the steps of the hamster
            switch (mCurrFrame)
            {
            case (static_cast<int>(FRAMES::STANDING)):
                mCurrFrame = static_cast<int>(FRAMES::RIGHT_STEP);
                mSound->playSoftStep();
                mPosition.x += STEP_SIZE;
                mPosition.y += STEP_SIZE / 8;
                mFrameTime = 0;
                break;
            case (static_cast<int>(FRAMES::RIGHT_STEP)):
                mCurrFrame = static_cast<int>(FRAMES::LEFT_STEP);
                mSound->playSoftStep();
                mPosition.x += STEP_SIZE;
                mPosition.y += STEP_SIZE / 8;
                mFrameTime = 0;
                break;
            case (static_cast<int>(FRAMES::LEFT_STEP)):
                mCurrFrame = static_cast<int>(FRAMES::RIGHT_STEP);
                mSound->playSoftStep();
                mPosition.x += STEP_SIZE;
                mPosition.y += STEP_SIZE / 8;
                mFrameTime = 0;
                break;
            case (static_cast<int>(FRAMES::SLEEPING)):
                mCurrFrame = static_cast<int>(FRAMES::STANDING);
                mFrameTime = 0;
                break;
            }

            // Change the state of the game when the hamster fully exits the saloon
            if (mPosition.x >= 400)
            {
                mCurrState = GameState::WALKING;
            }
        }
        break;

    case GameState::WALKING:
        // If a key was pressed
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
            // When the user hits space take a step or wake up
            case SDLK_SPACE:
                // Alternate the steps of the hamsters
                switch (mCurrFrame)
                {
                case (static_cast<int>(FRAMES::STANDING)):
                    mSound->playSoftStep();
                    mCurrFrame = static_cast<int>(FRAMES::RIGHT_STEP);
                    if (mDirectionForward)
                    {
                        mPosition.x += STEP_SIZE;
                        if (mPosition.x > RIGHT_WALL_POS)
                        {
                            mPosition.x = RIGHT_WALL_POS;
                        }
                    }
                    else
                    {
                        mPosition.x -= STEP_SIZE;
                        if (mPosition.x < LEFT_WALL_POS)
                        {
                            mPosition.x = LEFT_WALL_POS;
                        }
                    }

                    // The hamster will continue to approach the correct y position
                    if (mPosition.y < WALK_BASE_POS)
                    {
                        mPosition.y += STEP_SIZE / 8;
                    }
                    mFrameTime = 0;
                    break;
                case (static_cast<int>(FRAMES::RIGHT_STEP)):
                    mSound->playSoftStep();
                    mCurrFrame = static_cast<int>(FRAMES::LEFT_STEP);
                    if (mDirectionForward)
                    {
                        mPosition.x += STEP_SIZE;
                        if (mPosition.x > RIGHT_WALL_POS)
                        {
                            mPosition.x = RIGHT_WALL_POS;
                        }
                    }
                    else
                    {
                        mPosition.x -= STEP_SIZE;
                        if (mPosition.x < LEFT_WALL_POS)
                        {
                            mPosition.x = LEFT_WALL_POS;
                        }
                    }

                    // The hamster will continue to approach the correct y position
                    if (mPosition.y < WALK_BASE_POS)
                    {
                        mPosition.y += STEP_SIZE / 8;
                    }
                    mFrameTime = 0;
                    break;
                case (static_cast<int>(FRAMES::LEFT_STEP)):
                    mSound->playSoftStep();
                    mCurrFrame = static_cast<int>(FRAMES::RIGHT_STEP);
                    if (mDirectionForward)
                    {
                        mPosition.x += STEP_SIZE;
                        if (mPosition.x > RIGHT_WALL_POS)
                        {
                            mPosition.x = RIGHT_WALL_POS;
                        }
                    }
                    else
                    {
                        mPosition.x -= STEP_SIZE;
                        if (mPosition.x < LEFT_WALL_POS)
                        {
                            mPosition.x = LEFT_WALL_POS;
                        }
                    }

                    // The hamster will continue to approach the correct y position
                    if (mPosition.y < WALK_BASE_POS)
                    {
                        mPosition.y += STEP_SIZE / 8;
                    }
                    mFrameTime = 0;
                    break;
                case (static_cast<int>(FRAMES::SLEEPING)):
                    mCurrFrame = static_cast<int>(FRAMES::STANDING);
                    mFrameTime = 0;
                    break;
                }
                break;

            // When the user hits the right arrow change direction
            case SDLK_RIGHT:
                 mDirectionForward = true;
                 if (mCurrFrame == static_cast<int>(FRAMES::SLEEPING))
                 {
                     mCurrFrame = static_cast<int>(FRAMES::STANDING);
                 }
                 mFrameTime = 0;
                break;

            // When the user hits the left arrow change direction
            case SDLK_LEFT:
                mDirectionForward = false;
                if (mCurrFrame == static_cast<int>(FRAMES::SLEEPING))
                {
                    mCurrFrame = static_cast<int>(FRAMES::STANDING);
                }
                mFrameTime = 0;
                break;

            // When the user hits the up arrow
            case SDLK_UP:

                if (mCurrFrame == static_cast<int>(FRAMES::CLIMBING))
                {
                    mPosition.y = WHEEL_BASE_POS;
                    mDirectionForward = false;
                    mCurrFrame = static_cast<int>(FRAMES::STANDING);
                    mCurrState = GameState::WHEEL_STOPPED;
                }
                else if (mPosition.x >= WHEEL_LEFT_POS && mPosition.x <= WHEEL_RIGHT_POS)
                {
                    mCurrFrame = static_cast<int>(FRAMES::CLIMBING);
                    mPosition.x = CLIMB_UP_POS;
                }
                mFrameTime = 0;
                break;

            // When the user hits the down arrow
            case SDLK_DOWN:

                if (mCurrFrame == static_cast<int>(FRAMES::CLIMBING))
                {
                    mCurrFrame = static_cast<int>(FRAMES::STANDING);
                    mPosition.x = CLIMB_DOWN_POS;
                    mFrameTime = 0;
                }
                break;
            }
        }
        break;

    case GameState::WHEEL_STOPPED:
        //If a key was pressed
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
            // Exit the wheel when the player hits the down arrow
            case SDLK_DOWN:
                mCurrFrame = static_cast<int>(FRAMES::CLIMBING);
                mFrameTime = 0;
                mPosition.x = CLIMB_UP_POS;
                mPosition.y = WALK_BASE_POS;
                mCurrState = GameState::WALKING;
                break;

            // Begin the transition into the wheel playing state
            case SDLK_SPACE:
            if(mCurrFrame == static_cast<int>(FRAMES::SLEEPING))
            {
                mCurrFrame = static_cast<int>(FRAMES::STANDING);
            }

                mCurrState = GameState::WHEEL_PLAY_STARTING;
                mFrameTime = 0;
                break;
            }         
        }
        break;

    case GameState::WHEEL_PLAYING:
        //If the space key was pressed
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0 /*&& e.key.keysym.sym == SDLK_SPACE*/)
        {
            // Alternate the steps of the hamsters
            switch (mCurrFrame)
            {
            case (static_cast<int>(FRAMES::STANDING)):
                mCurrFrame = static_cast<int>(FRAMES::RIGHT_STEP);
                mFrameTime = 0;
                break;
            case (static_cast<int>(FRAMES::RIGHT_STEP)):
                mCurrFrame = static_cast<int>(FRAMES::LEFT_STEP);
                mFrameTime = 0;
                break;
            case (static_cast<int>(FRAMES::LEFT_STEP)):
                mCurrFrame = static_cast<int>(FRAMES::RIGHT_STEP);
                mFrameTime = 0;
                break;
            }
        }
        break;
    }
}

// Draw the hamster
void GHamster::render()
{
    if (mDirectionForward)
    {
        // Render the hamster
        mSpriteSheet.render(static_cast<int>(mPosition.x - (FRAME_WIDTH / 2.0)), static_cast<int>(mPosition.y - (FRAME_HEIGHT / 2.0)), &mAnimationFrames[mCurrFrame]);
    }
    else
    {
        mSpriteSheet.render(static_cast<int>(mPosition.x - (FRAME_WIDTH / 2.0)), static_cast<int>(mPosition.y - (FRAME_HEIGHT / 2.0)), &mAnimationFrames[mCurrFrame], 0, nullptr, SDL_FLIP_HORIZONTAL);
    }

    // Render the DustBall
    for (DustBall *tmpDustBall : mDustBalls)
    {
        tmpDustBall->render();
    }
}

// If the hamster is currently sleeping
bool GHamster::sleeping() { return (mCurrFrame == static_cast<int>(FRAMES::SLEEPING)); }

// Generate a new DustBall to the hamster
void GHamster::addDustBall()
{
    DustBall *tmpDustBall = new DustBall();
    tmpDustBall->init(mRenderer, UVector3{ mPosition.x + 65 + (rand() % 30), mPosition.y + 16 + (rand() % 20), 0 });
    mDustBalls.push_back(tmpDustBall);
}

// Deallocate the hamster's resources
void GHamster::free()
{
    // Prevent the sound from becoming a nullptr
    if (mSound)
    {
        mSound = nullptr;
    }

    // Free the DustBalls
    for (DustBall *tmpDustBall : mDustBalls)
    {
        tmpDustBall->free();
        delete tmpDustBall;
        tmpDustBall = nullptr;
    }
    mDustBalls.clear();

    // Free the hamsters spritesheet
    mSpriteSheet.free();
}

const float DustBall::LIVE_TIME = .9f;    // How long the DustBall exists before  being deleted
const int DustBall::X_RAND_VEL = 300;     // Used to randomly generate the initial X velocity of the DustBall
const int DustBall::Y_RAND_VEL = 130;     // Used to randomly generate the initial Y velocity of the DustBall
const int DustBall::DB_FRAME_WIDTH = 40;  // The width of the DustBall Frame
const int DustBall::DB_FRAME_HEIGHT = 40; // The height of the DustBall Frame

// Default intialize the DustBall values
DustBall::DustBall()
{
    mTime = 0;
    live = false;
    mVel = mPosition = UVector3{ 0,0,0 };
    mCurrFrame = mInitialFrame = 0;
    for (int i = 0; i < DB_FRAME_COUNT; ++i)
    {
        mDBAnimationFrames[i] = SDL_Rect();
    }
}

// Initialize the DustBall
bool DustBall::init(SDL_Renderer *aRenderer, const UVector3 &aInitialPosition)
{
    bool success = true;
    live = true;
    
    // Load the texture
    mTexture.initUTexture(aRenderer);
    if(!mTexture.loadFromFile("assets/dustball.png"))
    {
        printf("Failed to load dustball texture!\n");
        success = false;
    }
    else
    {
        mTexture.setBlendMode(SDL_BLENDMODE_BLEND);
        mTexture.setAlpha(255);
    }
   
    // Set the postiion of the DustBall
    mPosition = aInitialPosition;

    // Initialize DustBall's velocity and set the time to zero
    mTime = 0;
    mVel.x = (static_cast<double>(rand() % X_RAND_VEL));
    mVel.y = -(static_cast<double>(rand() % Y_RAND_VEL) - 20);

    // Initialize the DustBall frames
    for (int i = 0; i < DB_FRAME_COUNT; ++i)
    {
        mDBAnimationFrames[i].x = i * DB_FRAME_WIDTH;
        mDBAnimationFrames[i].y = 0;
        mDBAnimationFrames[i].w = DB_FRAME_WIDTH;
        mDBAnimationFrames[i].h = DB_FRAME_HEIGHT;
    }

    mInitialFrame = mCurrFrame = rand() % 3;

    // Return the success of this initialization
    return success;
}

// Render the DustBall
void DustBall::render()
{
    mTexture.render(mPosition.x - (DB_FRAME_WIDTH / 2), mPosition.y - (DB_FRAME_HEIGHT / 2), &mDBAnimationFrames[mCurrFrame]);
}

// Update the DustBall
void DustBall::update(const float &dt)
{
    // If the DustBall is still live
    if (mTime < LIVE_TIME)
    {
        // Determine the opacity of the dust ball based on the time it has been alive, then set the opacity of the DustBall
        Uint8 opacity = static_cast<Uint8>(SDL_MAX_UINT8 * (1.0 - (mTime / LIVE_TIME)));
        
        mTexture.setAlpha(opacity);

        // Update the time
        mTime += dt;

        // For the first 1/3 of the DustBalls life render the first frame
        if (mTime <= (LIVE_TIME / 3.0))
        {
            mCurrFrame = mInitialFrame;
        }

        // For the second 1/3 of the DustBalls life render the second frame
        else if (mTime <= (LIVE_TIME * 2.0 / 3.0))
        {
            mCurrFrame = mInitialFrame + 1;
            mCurrFrame %= 3;
        }

        // For the rest of the DustBalls life render the third frame
        else
        {
            mCurrFrame = mInitialFrame + 2;
            mCurrFrame %= 3;
        }

        // Update the position of the DustBall
        mPosition.x += (mVel.x * dt);
        mPosition.y += (mVel.y * dt);
    }
    // Otherwise mark the DustBall for deletion
    else
    {
        live = false;
    }
}

// Deallocate the resources used by the DustBall
void DustBall::free()
{
    mTexture.free();
}