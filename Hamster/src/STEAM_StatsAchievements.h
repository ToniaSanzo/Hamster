/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 4/17/2021
* File: Hamster STEAM_StatsAchievements.h
*/
#pragma once
#include "../../Steam/steam_api.h"
#include "../../Steam/isteamuser.h"
#include "../../Steam/isteamuserstats.h"
#include "../../Steam/steam_api_common.h"

enum Achievements
{
    ACH_FIRST_GAME    = 0,
    ACH_FIRST_RUN     = 1,
    ACH_FAST_RUN      = 2,
    ACH_LONG_DISTANCE = 3,
};

struct Achievement_t
{
	Achievements mAchievementID;
	const char *mAchievementIDChar;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};

class STEAM_StatsAchievements
{
public:
	// Number of unique achievements in the game
	const static int ACH_COUNT = 4;

	// Constructor
	STEAM_StatsAchievements();

	// Run a frame. Does not need to run at full frame rate.
	void RunFrame();

	// Display the stats and achievements
	void Render();

	// Accumulators
	void AddDistanceTraveled(float flDistance);

	// Accessors
	float GetTotalRuns() { return mTotalRuns; }
	double GetTotalLoops() { return mTotalLoops; }

	// Our Steam callbacks
	STEAM_CALLBACK(STEAM_StatsAchievements, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(STEAM_StatsAchievements, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(STEAM_StatsAchievements, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);

private:

	// Determine if we get this achievement now
	void EvaluateAchievement(Achievement_t &achievement);
	void UnlockAchievement(Achievement_t &achievement);

	// Store stats
	void StoreStatsIfNecessary();

	// Our Game Id
	CGameID mGameId;

	// Steam User interface
	ISteamUser *mSteamUser;

	// Steam UserStats interface
	ISteamUserStats *mSteamUserStats;

	// Did we get the stats from Steam?
	bool m_bRequestedStats;
	bool m_bStatsValid;

	// Should we store stats this frame?
	bool m_bStoreStats;

	// Current Stat details
	int mLoopsLastRun;

	// Persisted Stat details
	int mGamesPlayed;
	int mTotalRuns;
	int mTotalLoops;
};

