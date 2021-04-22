/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 4/17/2021
* File: Hamster STEAM_StatsAchievements.cpp
*/
#include "STEAM_StatsAchievements.h"

#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }

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
    mRenderer(nullptr),
    mSteamUser(nullptr),
    mSteamUserStats(nullptr),
    mGameId(SteamUtils()->GetAppID()),
    m_CallbackUserStatsReceived(this, &STEAM_StatsAchievements::OnUserStatsReceived),
    m_CallbackUserStatsStored(this, &STEAM_StatsAchievements::OnUserStatsStored),
    m_CallbackAchievementStored(this, &STEAM_StatsAchievements::OnAchievementStored)
{
    m_bRequestedStats = false;
    m_bStatsValid = false;
    m_bStoreStats = false;

    mLoopsLastRun = 0;
    
    mGamesPlayed = 0;
    mTotalRuns = 0;
    mTotalLoops = 0;
}

// Initialize the STEAM_StatsAchievements
bool STEAM_StatsAchievements::init(SDL_Renderer *aRenderer)
{
    // Initialize the success flag
    bool success = true;

    // Set SteamUser interface
    mSteamUser = SteamUser();
    // If setting up SteamUser interface failed
    if (mSteamUser == nullptr)
    {
        printf("Failed to load Steam User interface!\n");
        success = false;
    }

    // else SteamUser setup 
    else
    {
        mSteamUserStats = SteamUserStats();
        if (mSteamUserStats == nullptr)
        {
            printf("Failed to load Steam UserStats interface!\n");
            success = false;
        }
        else
        {
            mRenderer = aRenderer;
            if (mRenderer == nullptr)
            {
                printf("Did not pass in a valid renderer when initializing STEAM_StatsAchievements!\n");
                success = false;
            }
        }
    }

    return success;
}

// Run a frame. Does not need to run at full frame rate.
void STEAM_StatsAchievements::RunFrame()
{
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
        EvaluateAchievement(g_rgAchievements[iAch]);
    }

    // Store stats
    StoreStatsIfNecessary();
}

// Accumulators
void STEAM_StatsAchievements::AddLoops(int nLoops)
{
    mLoopsLastRun = nLoops;
    mTotalLoops += nLoops;
}
void STEAM_StatsAchievements::AddRun()
{
    ++mTotalRuns;
}

// See if we should unlock this achievement
void STEAM_StatsAchievements::EvaluateAchievement(Achievement_t &achievement)
{
    // Already achieved, done
    if (achievement.m_bAchieved)
        return;

    switch (achievement.mAchievementID)
    {
    case ACH_FIRST_GAME:
        UnlockAchievement(achievement);
        break;
    case ACH_FIRST_RUN:
        if (mTotalRuns >= 1)
            UnlockAchievement(achievement);
        break;
    case ACH_FAST_RUN:
        if (mLoopsLastRun >= 450)
            UnlockAchievement(achievement);
        break;
    case ACH_LONG_DISTANCE:
        if (mTotalLoops >= 33000)
            UnlockAchievement(achievement);
        break;
    default:
        break;
    }
}

// Unlock this achievement
void STEAM_StatsAchievements::UnlockAchievement(Achievement_t &achievement)
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
void STEAM_StatsAchievements::StoreStatsIfNecessary()
{
    if (m_bStoreStats)
    {
        // already set any achievements in UnlockAchievement

        // set stats
        mSteamUserStats->SetStat("GamesPlayed", mGamesPlayed);
        mSteamUserStats->SetStat("TotalRuns", mTotalRuns);
        mSteamUserStats->SetStat("TotalLoops", mTotalLoops);

        bool bSuccess = mSteamUserStats->StoreStats();
        // If this failed, we never sent anything to the server, try
        // again later.
        m_bStoreStats = !bSuccess;
    }
}


// We have recieved stats data from Steam. We then immediately update our data.
void STEAM_StatsAchievements::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
    printf("OnUserStatsReceived Callback\n");
    if (!mSteamUserStats)
        return;

    // we may get callbacks for other games' stats arriving, ignore them
    if (mGameId.ToUint64() == pCallback->m_eResult)
    {
        if (k_EResultOK == pCallback->m_eResult)
        {
            printf("Received stats and achievements from Steam\n");
            m_bStatsValid = true;

            // load achievements
            for (int iAch = 0; iAch < ACH_COUNT; ++iAch)
            {
                Achievement_t &ach = g_rgAchievements[iAch];
                mSteamUserStats->GetAchievement(ach.mAchievementIDChar, &ach.m_bAchieved);
                printf("[Achievement #%d]Name: %s, Desc: %s, Unlocked: %s\n", iAch, mSteamUserStats->GetAchievementDisplayAttribute(ach.mAchievementIDChar, "name"), mSteamUserStats->GetAchievementDisplayAttribute(ach.mAchievementIDChar, "desc"), ach.m_bAchieved ? "true" : "false");
            }

            // load stats
            mSteamUserStats->GetStat("GamesPlayed", &mGamesPlayed);
            mSteamUserStats->GetStat("TotalRuns", &mTotalRuns);
            mSteamUserStats->GetStat("TotalLoops", &mTotalLoops);
        }
        else
        {
            printf("RequestStats - failed, %d\n", pCallback->m_eResult);
        }
    }
}

// Our stats data was stored!
void STEAM_StatsAchievements::OnUserStatsStored(UserStatsStored_t *pCallback)
{
    printf("OnUserStatsStored Callback\n");

    // We may get callbacks for other games' stats arriving, ignore them
    if (mGameId.ToUint64() == pCallback->m_nGameID)
    {
        if (k_EResultOK == pCallback->m_eResult)
        {
            printf("StoreStats - success\n");
        }
        else if (k_EResultInvalidParam == pCallback->m_eResult)
        {
            // One or more stats we set broke a constraint. They've been reverted,
            // and we should re-iterate the values now to keep in sync.
            printf("StoreStats - some failed to validate\n");
            // Fake up a callback here so that we re-load the values.
            UserStatsReceived_t callback;
            callback.m_eResult = k_EResultOK;
            callback.m_nGameID = mGameId.ToUint64();
            OnUserStatsReceived(&callback);
        }
        else
        {
            printf("StoreStats - failed, %d\n", pCallback->m_eResult);
        }
    }
}

// An achievements was stored
void STEAM_StatsAchievements::OnAchievementStored(UserAchievementStored_t *pCallback)
{
    printf("OnAchievementStored Callback\n");

    // we may get callbacks for other games stats arriving, ignore them
    if (mGameId.ToUint64() == pCallback->m_nGameID)
    {
        if (0 == pCallback->m_nMaxProgress)
        {
            printf("Achievement '%s' unlocked!", pCallback->m_rgchAchievementName);
        }
        else
        {
            printf("Achievement '%s' progress callback, (%d / %d)\n", pCallback->m_rgchAchievementName, pCallback->m_nCurProgress, pCallback->m_nMaxProgress);
        }
    }
}

// Free allocated resources
void STEAM_StatsAchievements::free()
{
    mSteamUser = nullptr;
    mSteamUserStats = nullptr;
    mRenderer = nullptr;
}