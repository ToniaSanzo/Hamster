/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 4/29/2021
* File: Hamster STEAM_Leaderbaords.h
*/
#pragma once
#include "STEAM_StatsAchievements.h"

// A leaderboard item
struct LeaderboardMenuItem_t
{
    bool m_bBack;
    bool m_bNextLeaderboard;
};

class STEAM_Leaderboards
{
public:
    // Constructor
    STEAM_Leaderboards();

    // Initialize the STEAM_Leaderboard
    bool init();

    // Run a frame. Does not need t orun at full frame rate.
    void update(const float &);

    // Updates leaderboards with stats from our just finished game
    void UpdateLeaderboards(STEAM_StatsAchievements *pStats, bool bNewHighScore);

private:
    void FindLeaderboards();

    // Called when SteamUserStats()->FindOrCreateLeaderboard() returns asynchronously
    void OnFindLeaderboard(LeaderboardFindResult_t *pFindLeaderboardResult, bool bIOFailure);
    CCallResult<STEAM_Leaderboards, LeaderboardFindResult_t> m_SteamCallResultCreateLeaderboard;

    // Called when SteamUserStats()->UploadLeaderboardScore() returns asynchronously
    void OnUploadScore(LeaderboardScoreUploaded_t *pFindLeaderboardResult, bool bIOFailure);
    CCallResult<STEAM_Leaderboards, LeaderboardScoreUploaded_t> m_SteamCallResultUploadScore;

    // Handles to our leaderboards
    SteamLeaderboard_t m_hFastestRunLeaderboard;
    SteamLeaderboard_t m_hLongDistanceLeaderboard;

    int m_bLoading;                            // True if we're looking up a leaderboard handle

    int m_nCurrentLeaderboard;                 // Index for leaderboard the user is currently viewing 
};
