/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 4/17/2021
* File: Hamster STEAM_StatsAchievements.cpp
*/
#include "STEAM_StatsAchievements.h"
#include "ULib.h"
#include <cassert>
#include <cmath>

#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }


#ifndef ACH_COUNT 
#define ACH_COUNT 4 // Achievement count
#endif


#ifndef ACH_RENDER_TIME 
#define ACH_RENDER_TIME 4.5f // Achievement render time
#endif

Achievement_t g_rgAchievements[] =
{
    _ACH_ID(ACH_FIRST_GAME, "Good Morning"),
    _ACH_ID(ACH_FIRST_RUN, "Back to Basics"),
    _ACH_ID(ACH_FAST_RUN, "Super Speed"),
    _ACH_ID(ACH_LONG_DISTANCE, "Marathon Runner"),
};

// Constructor
STEAM_StatsAchievements::STEAM_StatsAchievements()
    :
    mSteamUser(nullptr),
    mSteamUserStats(nullptr),
    mGameId(SteamUtils()->GetAppID()),
    m_CallbackUserStatsReceived(this, &STEAM_StatsAchievements::onUserStatsReceived),
    m_CallbackUserStatsStored(this, &STEAM_StatsAchievements::onUserStatsStored),
    m_CallbackAchievementStored(this, &STEAM_StatsAchievements::onAchievementStored)
{
    m_bRequestedStats = false;
    m_bStatsValid = false;
    m_bStoreStats = false;
    m_bNewGame = true;

    m_flCurrAchTime = 0.f;
    m_bFullyOpaque = false;

    mLoopsLastRun = 0;

    m_shUpdateCount = 0;
    
    mGamesPlayed = 0;
    mTotalRuns = 0;
    mTotalLoops = 0;
}

// Initialize the STEAM_StatsAchievements
bool STEAM_StatsAchievements::init()
{
    // Initialize the success flag
    bool success = true;

    // Set SteamUser interface
    mSteamUser = SteamUser();
    // If setting up SteamUser interface failed
    if (mSteamUser == nullptr)
    {
        // printf("Failed to load Steam User interface!\n");
        success = false;
    }

    // Otherwise continue setup 
    else
    {
        // Set SteamUserStats interface
        mSteamUserStats = SteamUserStats();
        // If setting up SteamUserStats interface failed
        if (mSteamUserStats == nullptr)
        {
            // printf("Failed to load Steam UserStats interface!\n");
            success = false;
        }
    }

    return success;
}

// Run a frame. Does not need to run at full frame rate.
void STEAM_StatsAchievements::update(const float &dt)
{
    // If the update count is not 60, exit
    if ((++m_shUpdateCount) < 60)
        return;

    m_shUpdateCount = 0;
    SteamAPI_RunCallbacks();
    if (!m_bRequestedStats)
    {
        // Is Steam Loaded? if no, can't get stats, done
        if (nullptr == mSteamUserStats || nullptr == mSteamUser)
        {
            m_bRequestedStats = true;
            return;
        }

        // If yes, request our stats
         bool bSuccess = mSteamUserStats->RequestCurrentStats();

        // This function  should only return galse if we werent logged in, and we 
        // already checked that. But handle it being false again anyway, just ask 
        // again later.
        m_bRequestedStats = bSuccess;
    }

    // If callback stats haven't been called, done
    if (!m_bStatsValid)
        return;

    // Get info from sources

    // Evaluate achievements
    for (int iAch = 0; iAch < ACH_COUNT; ++iAch)
    {
        evaluateAchievement(g_rgAchievements[iAch]);
    }

    // Store stats
    storeStatsIfNecessary();
}

// Accumulators
void STEAM_StatsAchievements::addLoops(int nLoops)
{
    mTotalLoops += nLoops;
    mLoopsLastRun = nLoops;
    ++mTotalRuns;
}

// See if we should unlock this achievement
void STEAM_StatsAchievements::evaluateAchievement(Achievement_t &achievement)
{
    // Already achieved, done
    if (achievement.m_bAchieved)
        return;

    switch (achievement.mAchievementID)
    {
    case ACH_FIRST_GAME:
        unlockAchievement(achievement);
        break;
    case ACH_FIRST_RUN:
        if (mTotalRuns >= 1)
        {
            unlockAchievement(achievement);
        }
        break;
    case ACH_FAST_RUN:
        if (mLoopsLastRun >= 405)
        {
            unlockAchievement(achievement);
        }
        break;
    case ACH_LONG_DISTANCE:
        if (mTotalLoops >= 3300)
        {
            unlockAchievement(achievement);
        }
        break;
    default:
        break;
    }
}

// Unlock this achievement
void STEAM_StatsAchievements::unlockAchievement(Achievement_t &achievement)
{
    achievement.m_bAchieved = true;

    // the icon may change once it's unlocked
    achievement.m_iIconImage = 0;

    // mark it down
    mSteamUserStats->SetAchievement(achievement.mAchievementIDChar);

    // Store stats end of frame
    m_bStoreStats = true;
}

// Store stats in the Steam database
void STEAM_StatsAchievements::storeStatsIfNecessary()
{
    if (m_bStoreStats)
    {
        // already set any achievements in UnlockAchievement

        // set stats
        mSteamUserStats->SetStat("TotalRuns", mTotalRuns);
        mSteamUserStats->SetStat("TotalLoops", mTotalLoops);

        bool bSuccess = mSteamUserStats->StoreStats();
        // If this failed, we never sent anything to the server, try
        // again later.
        m_bStoreStats = !bSuccess;
    }
}

// We have recieved stats data from Steam. We then immediately update our data.
void STEAM_StatsAchievements::onUserStatsReceived(UserStatsReceived_t *pCallback)
{
    // printf("OnUserStatsReceived Callback\n");
    m_bUsersStatsRecieved = true;
    if (!mSteamUserStats)
        return;

    // we may get callbacks for other games' stats arriving, ignore them
    if (mGameId.ToUint64() == pCallback->m_nGameID)
    {
        if (k_EResultOK == pCallback->m_eResult)
        {
            // printf("Received stats and achievements from Steam\n");
            m_bStatsValid = true;

            // load achievements
            for (int iAch = 0; iAch < ACH_COUNT; ++iAch)
            {
                Achievement_t &ach = g_rgAchievements[iAch];
                mSteamUserStats->GetAchievement(ach.mAchievementIDChar, &ach.m_bAchieved);
                // printf("[Achievement #%d]Name: %s, Desc: %s, Unlocked: %s\n", iAch, mSteamUserStats->GetAchievementDisplayAttribute(ach.mAchievementIDChar, "name"), mSteamUserStats->GetAchievementDisplayAttribute(ach.mAchievementIDChar, "desc"), ach.m_bAchieved ? "true" : "false");
            }

            // load stats
            mSteamUserStats->GetStat("GamesPlayed", &mGamesPlayed);
            mSteamUserStats->GetStat("TotalRuns", &mTotalRuns);
            mSteamUserStats->GetStat("TotalLoops", &mTotalLoops);

            // Increment the number of games played if this is a new game
            if (m_bNewGame)
            {
                ++mGamesPlayed;
                mSteamUserStats->SetStat("GamesPlayed", mGamesPlayed);
                storeStatsIfNecessary();
                m_bNewGame = false;
            }
        }
        else
        {
            // printf("RequestStats - failed, %d\n", pCallback->m_eResult);
        }
    }
}

// Our stats data was stored!
void STEAM_StatsAchievements::onUserStatsStored(UserStatsStored_t *pCallback)
{
    // printf("OnUserStatsStored Callback\n");

    // We may get callbacks for other games' stats arriving, ignore them
    if (mGameId.ToUint64() == pCallback->m_nGameID)
    {
        if (k_EResultOK == pCallback->m_eResult)
        {
            // printf("StoreStats - success\n");
        }
        else if (k_EResultInvalidParam == pCallback->m_eResult)
        {
            // One or more stats we set broke a constraint. They've been reverted,
            // and we should re-iterate the values now to keep in sync.
            // printf("StoreStats - some failed to validate\n");
            // Fake up a callback here so that we re-load the values.
            UserStatsReceived_t callback;
            callback.m_eResult = k_EResultOK;
            callback.m_nGameID = mGameId.ToUint64();
            onUserStatsReceived(&callback);
        }
        else
        {
            // printf("StoreStats - failed, %d\n", pCallback->m_eResult);
        }
    }
}

// An achievements was stored
void STEAM_StatsAchievements::onAchievementStored(UserAchievementStored_t *pCallback)
{
    // printf("OnAchievementStored Callback\n");

    // we may get callbacks for other games stats arriving, ignore them
    if (mGameId.ToUint64() == pCallback->m_nGameID)
    {
        if (0 == pCallback->m_nMaxProgress)
        {
            // printf("Achievement '%s' unlocked!", pCallback->m_rgchAchievementName);
        }
        else
        {
            // printf("Achievement '%s' progress callback, (%d / %d)\n", pCallback->m_rgchAchievementName, pCallback->m_nCurProgress, pCallback->m_nMaxProgress);
        }
    }
}

// Clear all stats and achievements
void STEAM_StatsAchievements::clearStatsAchievements()
{
    mSteamUserStats->ResetAllStats(true);
}

// Free allocated resources
void STEAM_StatsAchievements::free()
{
    // Prevent dangling pointers, their resources are deallocated later in the program
    mSteamUserStats = nullptr;
    mSteamUser = nullptr;
}