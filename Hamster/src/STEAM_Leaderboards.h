/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 4/29/2021
* File: Hamster STEAM_Leaderbaords.h
*/
#pragma once
#include "STEAM_StatsAchievements.h"
#include "GButton.h"

// A leaderboard item
struct LeaderboardMenuItem_t
{
    bool m_bBack;
    bool m_bNextLeaderboard;
};

class STEAM_LeaderboardMenu;

class STEAM_Leaderboards
{
public:
    // Constructor
    STEAM_Leaderboards();
    // Destructor
    ~STEAM_Leaderboards();

    /**
    * Properly initialize the leaderboard menu
    * 
    * @param {SDL_Renderer*} a_pRenderer reference to the game renderer
    * @return {bool} If the initialization was succesful
    */
    bool init(SDL_Renderer*);

    /**
    * Run a frame for the STEAM_Leaderboards
    * @param {float} dt delta time, time passed since last frame was run
    */
    void update(const float &);

    // Shows / refreshes leaderboard (fastest run)
    void ShowFastestRun();

    // Shows / refreshes leaderboard (longest distance)
    void ShowLongestDistance();

    // Render the leaderboard menu's
    void render();

    // Updates leaderboards with stats from our just finished game
    void UpdateLeaderboards(STEAM_StatsAchievements *pStats);

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

    STEAM_LeaderboardMenu* m_pLeaderboardMenu; // Displays the current leaderboard
    int m_nCurrentLeaderboard;                 // Index for leaderboard the user is currently viewing 

    bool m_bRenderFastRunLeaderboard;          // If true render fast run leaderboard, otherwise render long distance leaderboard

    GButton m_btnDirectionArrow;               // Button alternates between leaderboards
};
