/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 4/17/2021
* File: Hamster STEAM_StatsAchievements.h
*/
#pragma once
#include "../../Steam/steam_api.h"


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
	const char *mAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};

class STEAM_StatsAchievements
{
public:
	// Constructore
	STEAM_StatsAchievements();


private:
	// Our Game Id
	CGameID

	// Persisted Stat details
	int mGamesPlayed;
	int mTotalRuns;
	int mTotalLoops;
};

