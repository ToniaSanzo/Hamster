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
    mSteamUser(nullptr),
    mSteamUserStats(nullptr),
    mGameId(SteamUtils()->GetAppID()),
    m_CallbackUserStatsReceived(this, &STEAM_StatsAchievements::OnUserStatsReceived),
    m_CallbackUserStatsStored(this, &STEAM_StatsAchievements::OnUserStatsStored),
    m_CallbackAchievementStored(this, &STEAM_StatsAchievements::OnAchievementStored)
{
    mSteamUser = SteamUser();
    mSteamUserStats = SteamUserStats();

    m_bRequestedStats = false;
    m_bStatsValid = false;
    m_bStoreStats = false;

    mLoopsLastRun = 0;
    
    mGamesPlayed = 0;
    mTotalRuns = 0;
    mTotalLoops = 0;
}

// We have recieved stats data from Steam. We then immediately update our data.
void STEAM_StatsAchievements::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
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
                printf("[Achievement]Name: %s, Desc: %s\n", mSteamUserStats->GetAchievementDisplayAttribute(ach.mAchievementIDChar, "name"), mSteamUserStats->GetAchievementDisplayAttribute(ach.mAchievementIDChar, "desc"));
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