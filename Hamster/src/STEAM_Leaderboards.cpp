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

// Menu that shows a leaderboard
class STEAM_LeaderboardMenu
{
    static const int k_nMaxLeaderboardEntries = 10;						// maximum number of leaderboard entries we can display
    LeaderboardEntry_t m_leaderboardEntries[k_nMaxLeaderboardEntries];	// leaderboard entries we received from DownloadLeaderboardEntries
    int m_nLeaderboardEntries;											// number of leaderboard entries we received

    SteamLeaderboard_t m_hSteamLeaderboard;			// handle to the leaderboard we are displaying
    ELeaderboardDataRequest m_eLeaderboardData;		// type of data we are displaying
    bool m_bLoading;								// waiting to receive leaderboard results
    bool m_bIOFailure;								// last attempt to retrieve the leaderboard failed

    CCallResult<STEAM_LeaderboardMenu, LeaderboardScoresDownloaded_t> m_callResultDownloadEntries;

    // Leaderboard menu texture assets
    UTexture m_texFastRunBoard, m_texLongDistanceBoard, m_texExitBtn;
public:

    // Constructor
    STEAM_LeaderboardMenu()
    {
        m_eLeaderboardData = k_ELeaderboardDataRequestGlobal;
        m_hSteamLeaderboard = 0;
        m_nLeaderboardEntries = 0;
        m_bLoading = false;
        m_bIOFailure = false;
    }

    // Destructor
    ~STEAM_LeaderboardMenu()
    {
        m_texFastRunBoard.free();
        m_texLongDistanceBoard.free();
        m_texExitBtn.free();
    }

    // Initialize the Leaderboard menu's textures
    bool init(SDL_Renderer* a_pRenderer)
    {
        // Success is true if m_pRenderer is valid
        bool success = a_pRenderer;
        if (success)
        {
            m_texFastRunBoard.initUTexture(a_pRenderer);
            if (!m_texFastRunBoard.loadFromFile("assets/fastest_run_leaderboard.png"))
            {
                std::printf("Failed to load fastest run leaderboard!\n");
                success = false;
            }

            m_texLongDistanceBoard.initUTexture(a_pRenderer);
            if (!m_texLongDistanceBoard.loadFromFile("assets/longest_distance_leaderboard.png"))
            {
                std::printf("Failed to load longest distance leaderboard!\n");
                success = false;
            }

            m_texExitBtn.initUTexture(a_pRenderer);
            if (!m_texExitBtn.loadFromFile("assets/x_button.png"))
            {
                std::printf("Failed to load exit button!\n");
                success = false;
            }
        }
        return success;
    }

    // Menu that shows a leaderboard
    void ShowLeaderboard(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardData, int offset)
    {
        m_hSteamLeaderboard = hLeaderboard;
        m_eLeaderboardData = eLeaderboardData;
        m_bLoading = true;
        m_bIOFailure = false;

        if (hLeaderboard)
        {
            // load the specified leaderboard data. We only display k_nMaxLeaderboardEntries entries at a time
            SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, eLeaderboardData,
                offset, offset + k_nMaxLeaderboardEntries);

            // Register for the async callback
            m_callResultDownloadEntries.Set(hSteamAPICall, this, &STEAM_LeaderboardMenu::OnLeaderboardDownloadedEntries);
        }

        Rebuild();
    }

    // Render the leaderboard menu
    void Render(bool a_bRenderFastRunLeaderboard) 
    {
        if (a_bRenderFastRunLeaderboard)
        {
            m_texFastRunBoard.render((ULib::SCREEN_DIMENSIONS.x - m_texFastRunBoard.getWidth()) / 2.f, (ULib::SCREEN_DIMENSIONS.y - m_texFastRunBoard.getHeight()) / 2.f);
        }
        else
        {
            m_texLongDistanceBoard.render((ULib::SCREEN_DIMENSIONS.x - m_texLongDistanceBoard.getWidth()) / 2.f, (ULib::SCREEN_DIMENSIONS.y - m_texLongDistanceBoard.getHeight()) / 2.f);
        }
    }

    // Creates a leaderboard menu
    void Rebuild()
    {
        if (m_hSteamLeaderboard)
        {
            // create a header for the leaderboard

            std::string strName = "Leaderboard: ";
            strName += SteamUserStats()->GetLeaderboardName(m_hSteamLeaderboard);

            if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobal)
                strName += ", Top 10\n";
            else if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobalAroundUser)
                strName += ", Around User\n";
            else if (m_eLeaderboardData == k_ELeaderboardDataRequestFriends)
                strName += ", Friends of User\n";

            printf(strName.c_str());
        }

        // Create leaderboard
        if (!m_hSteamLeaderboard || m_bLoading)
        {
            printf("Loading...\n");
        }
        else if (m_bIOFailure)
        {
            printf("Network failure!\n");
        }
        else
        {
            if (m_nLeaderboardEntries == 0)
            {
                // Requesting for global scores around the user will return successfully with 0 results if the
                // user does not have an entry on the leaderboard

                std::string strText;         
                if (m_eLeaderboardData != k_ELeaderboardDataRequestGlobalAroundUser)
                {
                    strText = "No scores for this leaderboard\n";
                }
                else
                {
                    strText = SteamFriends()->GetPersonaName();
                    strText += " does not have a score for this leaderboard\n";
                }

                printf(strText.c_str());
            }

            for (int index = 0; index < m_nLeaderboardEntries; index++)
            {
                std::string name(SteamFriends()->GetFriendPersonaName(m_leaderboardEntries[index].m_steamIDUser));
                printf("(%d) %s - %d\n", m_leaderboardEntries[index].m_nGlobalRank, name.c_str(), m_leaderboardEntries[index].m_nScore);
            }
        }  
    }

    // Called when SteamUserStats()->DownloadLeaderboardEntries() returns asynchronously
    void OnLeaderboardDownloadedEntries(LeaderboardScoresDownloaded_t* pLeaderboardScoresDownloaded, bool bIOFailure)
    {
        m_bLoading = false;
        m_bIOFailure = bIOFailure;

        // leaderboard entries handle will be invalid once we return from this function. Copy all data now.
        m_nLeaderboardEntries = MIN(pLeaderboardScoresDownloaded->m_cEntryCount, k_nMaxLeaderboardEntries);
        for (int index = 0; index < m_nLeaderboardEntries; index++)
        {
            SteamUserStats()->GetDownloadedLeaderboardEntry(pLeaderboardScoresDownloaded->m_hSteamLeaderboardEntries,
                index, &m_leaderboardEntries[index], NULL, 0);
        }

        // show our new data
        Rebuild();
    }
};

// Constructor
STEAM_Leaderboards::STEAM_Leaderboards()
{
    m_hFastestRunLeaderboard = 0;
    m_hLongDistanceLeaderboard = 0;
    m_nCurrentLeaderboard = 0;

    m_bLoading = false;
    m_bRenderFastRunLeaderboard = true;
    m_pLeaderboardMenu = nullptr;

    FindLeaderboards();
}

// Destructor
STEAM_Leaderboards::~STEAM_Leaderboards()
{
    if (m_pLeaderboardMenu)
    {
        delete m_pLeaderboardMenu;
    }
}

// Properly initialize the leaderboard menu
bool STEAM_Leaderboards::init(SDL_Renderer* a_pRenderer)
{
    m_pLeaderboardMenu = new STEAM_LeaderboardMenu();
    return m_pLeaderboardMenu->init(a_pRenderer);
}

// Run a frame for the STEAM_Leaderboards
void STEAM_Leaderboards::update(const float &dt)
{
    //printf("STEAM_Leaderboards RunFrame called.\n");
}

// Shows / refreshes leaderboard (fastest run)
void STEAM_Leaderboards::ShowFastestRun()
{
    // we want to show the top 10 fastest run. To do so, we request global score data beginning at 0
    m_pLeaderboardMenu->ShowLeaderboard(m_hFastestRunLeaderboard, k_ELeaderboardDataRequestGlobalAroundUser, -5);
}

// Shows / refreshes leaderboard (longest distance)
void STEAM_Leaderboards::ShowLongestDistance()
{
    // we want to show the 10 users around us
    m_pLeaderboardMenu->ShowLeaderboard(m_hLongDistanceLeaderboard, k_ELeaderboardDataRequestGlobalAroundUser, -5);
}

// Render the leaderboard menu
void STEAM_Leaderboards::render()
{
    m_pLeaderboardMenu->Render(m_bRenderFastRunLeaderboard);
}

// Gets handles for our leaderboards. If the leaderboards doesn't exist, creates them.
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
        m_SteamCallResultCreateLeaderboard.Set(hSteamAPICall, this, &STEAM_Leaderboards::OnFindLeaderboard);
        m_bLoading = true;
    }
}

// Called when SteamUserStats()->FindLeader() returns asynchronously
void STEAM_Leaderboards::OnFindLeaderboard(LeaderboardFindResult_t *pFindLeaderboardResult, bool bIOFailure)
{
    m_bLoading = false;

    // see if we encountered an error during the call
    if (!pFindLeaderboardResult->m_bLeaderboardFound || bIOFailure)
    {
        return;
    }

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
void STEAM_Leaderboards::UpdateLeaderboards(STEAM_StatsAchievements *pStats)
{
    // Each game the user finishes update the users longest distance leaderboard entry.
    if (m_hLongDistanceLeaderboard)
    {
        SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(m_hLongDistanceLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, (int)pStats->getTotalLoops(), NULL, 0);
        m_SteamCallResultUploadScore.Set(hSteamAPICall, this, &STEAM_Leaderboards::OnUploadScore);
    }

    // If the user has a new high score update the fast run leaderboard as well
    if (m_hFastestRunLeaderboard)
    {
        SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(m_hFastestRunLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, (int)pStats->getLoopsLastRun(), NULL, 0);
        m_SteamCallResultUploadScore.Set(hSteamAPICall, this, &STEAM_Leaderboards::OnUploadScore);
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