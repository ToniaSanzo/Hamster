/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 4/17/2021
* File: Hamster STEAM_StatsAchievements.h
*/
#pragma once
#include "ULib.h"
#include "UTexture.h"
#include <deque>
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
	// Constructor
	STEAM_StatsAchievements();

	// Initialize the STEAM_StatsAchievements
	bool init();

	// Run a frame. Does not need to run at full frame rate.
	void update(const float &);

	// Accumulators
	void addLoops(int nLoops);

	// Accessors
	int getTotalRuns() { return mTotalRuns; }
	int getTotalLoops() { return mTotalLoops; }
	int getGamesPlayed() { return mGamesPlayed; }
	int getLoopsLastRun() { return mLoopsLastRun; }

	// Clear all stats and achievements
	void clearStatsAchievements();

	// Free allocated resources
	void free();

	// used to prevent the user from clicking the trophies too soon
	bool m_bUsersStatsRecieved = false;

	// Our Steam callbacks
	STEAM_CALLBACK(STEAM_StatsAchievements, onUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(STEAM_StatsAchievements, onUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(STEAM_StatsAchievements, onAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);

private:

	// Determine if we get this achievement now
	void evaluateAchievement(Achievement_t &achievement);
	void unlockAchievement(Achievement_t &achievement);

	// Store stats
	void storeStatsIfNecessary();

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

	// Persisted Stat details
	int mGamesPlayed;
	int mTotalRuns;
	int mTotalLoops;
	int mLoopsLastRun;

	// Used to increment the mGamesPlayed count
	bool m_bNewGame;
	
	// Render helper variables
	float m_flCurrAchTime;
	bool m_bFullyOpaque;
	
	// Used to update the STEAM_StatsAchievements once every second
	unsigned short m_shUpdateCount;
};

