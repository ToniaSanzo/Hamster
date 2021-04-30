/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 4/29/2021
* File: Hamster STEAM_Leaderbaords.cpp
*/
#include "STEAM_Leaderboards.h"

// Leaderboard names
#define LEADERBOARD_FASTEST_RUN "FastestRun"
#define LEADERBOARD_LONGEST_DISTANCE "TotalLoops"

STEAM_Leaderboards::STEAM_Leaderboards()
{
    m_hFastestRunLeaderboard = 0;
    m_hLongDistanceLeaderboard = 0;
    m_nCurrentLeaderboard = 0;

    m_bLoading = false;
    
    FindLeaderboards();
}

// Run a frame for the STEAM_Leaderboards
void STEAM_Leaderboards::update(const float &dt)
{
    printf("STEAM_Leaderboards RunFrame called.\n");
}

// Gets handles for our leaderboards. If the leaderboards don't exist, creates them.
// Each time this is called, we look up another leaderboard.
void STEAM_Leaderboards::FindLeaderboards()
{
    if (m_bLoading)
        return;

    SteamAPICall_t hSteamAPICall = 0;

    if (!m_hFastestRunLeaderboard)
    {
        // Find the leaderboard for the fastest run
        hSteamAPICall = SteamUserStats()->FindLeaderboard(LEADERBOARD_FASTEST_RUN);
    }
    else if (!m_hLongDistanceLeaderboard)
    {
        // find/create a leaderboard for the user with the most loops
        hSteamAPICall = SteamUserStats()->FindLeaderboard(LEADERBOARD_LONGEST_DISTANCE);
    }

    if (hSteamAPICall != 0)
    {
        // set the function to call when this API call has completed
        m_SteamCallResultCreateLeaderboard.Set(hSteamAPICall, this, &OnFindLeaderboard);
        m_bLoading = true;
    }
}

// Called when SteamUserStats()->FindLeader() returns asynchronously
void STEAM_Leaderboards::OnFindLeaderboard(LeaderboardFindResult_t *pFindLeaderboardResult, bool bIOFailure)
{
    m_bLoading = false;

    // see if we encountered an error during the call
    if (!pFindLeaderboardResult->m_bLeaderboardFound || bIOFailure)
        return;

    // check to see which leaderboard handle we just retrieved
    const char *pchName = SteamUserStats()->GetLeaderboardName(pFindLeaderboardResult->m_hSteamLeaderboard);
    if (strcmp(pchName, LEADERBOARD_FASTEST_RUN) == 0)
    {
        m_hFastestRunLeaderboard = pFindLeaderboardResult->m_hSteamLeaderboard;
    }
    else if (strcmp(pchName, LEADERBOARD_LONGEST_DISTANCE) == 0)
    {
        m_hLongDistanceLeaderboard = pFindLeaderboardResult->m_hSteamLeaderboard;
    }

    // look up any other leaderboards
    FindLeaderboards();
}

// Updates leaderboards with stats from our just finished game
void STEAM_Leaderboards::UpdateLeaderboards(STEAM_StatsAchievements *pStats, bool bNewHighscore)
{
    // Each game the user finishes update the users longest distance leaderboard entry.
    if (m_hLongDistanceLeaderboard)
    {
        SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(m_hLongDistanceLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, (int)pStats->getTotalLoops(), NULL, 0);
        m_SteamCallResultUploadScore.Set(hSteamAPICall, this, &OnUploadScore);
    }

    // If the user has a new high score update the fast run leaderboard as well
    if (m_hFastestRunLeaderboard && bNewHighscore)
    {
        SteamUserStats()->UploadLeaderboardScore(m_hFastestRunLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, (int)pStats->getLoopsLastRun(), NULL, 0);
    }

}

// Called when SteamUserStats(0->UploadLeaderboardScore() returns asynchronously
void STEAM_Leaderboards::OnUploadScore(LeaderboardScoreUploaded_t *pScoreUploadedResult, bool bIOFailure)
{
    if (!pScoreUploadedResult->m_bSuccess)
    {
        printf("error\n");
    }

    if (pScoreUploadedResult->m_bScoreChanged)
    {
        printf("could display new rank");
    }
}


