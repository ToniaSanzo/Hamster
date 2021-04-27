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
	bool init(SDL_Renderer *);

	// Run a frame. Does not need to run at full frame rate.
	void update(const float &);

	// Display the stats and achievements
	void render();

	// Set the opacity of an achievement based on the time the achievement's been alive
	void setOpacity(UTexture *);

	// Giving an achievement id return the corresponding achievement texture
	UTexture* getAchTexture(const Achievements &);

	// Accumulators
	void addLoops(int nLoops);
	void addRun();

	// Accessors
	int getTotalRuns() { return mTotalRuns; }
	int getTotalLoops() { return mTotalLoops; }
	int getGamesPlayed() { return mGamesPlayed; }

	// Free allocated resources
	void free();

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

	// Pointer to the SDL renderer
	SDL_Renderer *mRenderer;

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

	// Achievement textures
	UTexture m_utFirstGameAch, m_utFirstRunAch, m_utFastRunAch, m_utLongDistanceAch;
	
	// Currently unlocked achievement references
	Achievements *m_enCurrAch;
	std::deque<Achievements*> m_dqUnlockedAch;
	
	// Render helper variables
	float m_flCurrAchTime;
	bool m_bFullyOpaque;
	
	// Used to update the STEAM_StatsAchievements once every second
	unsigned short m_shUpdateCount;
};

