/**
* Copyright 2021 Tonia Sanzo ©
* Title: Hamster
* Author: Tonia Sanzo
* Date: 4/29/2021
* File: Hamster STEAM_Leaderbaords.cpp
*/
#include "STEAM_Leaderboards.h"
#include "cassert"

// Leaderboard names
#define LEADERBOARD_FASTEST_RUN "FastestRun"
#define LEADERBOARD_LONGEST_DISTANCE "TotalLoops"

// Metadata about leaderboard entries
struct LeaderboardEntry
{
    ELeaderboardEntryOption m_eOption;
    std::string m_name;
    int32 m_nScore;
    int32 m_nGlobalRank;

    // Default Constructor
    LeaderboardEntry()
    {
        m_eOption = ELeaderboardEntryOption::k_EInvalid;
        m_name = "";
        m_nScore = 0;
        m_nGlobalRank = 0;
    }

    // Construct a valid leaderboard entry
    LeaderboardEntry(std::string a_name, int32 a_nScore, int32 a_nGlobalRank)
    {
        m_eOption = ELeaderboardEntryOption::k_EValid;
        m_name = a_name;
        m_nScore = a_nScore;
        m_nGlobalRank = a_nGlobalRank;
    } 

    // Construct an empty leaderboard entry, with a specified option
    LeaderboardEntry(ELeaderboardEntryOption a_eOption)
    {
        m_eOption = a_eOption;
        m_name = "";
        m_nScore = 0;
        m_nGlobalRank = 0;
    }
};

// Menu that shows a leaderboard
class STEAM_LeaderboardMenu
{
    static const int k_nMaxLeaderboardEntries = 11;		// maximum number of leaderboard entries we can display
    std::vector<LeaderboardEntry> m_leaderboardEntries;	// leaderboard entries we received from DownloadLeaderboardEntries
    LeaderboardEntry* m_usersEntry;                     // The current user's leaderboard entry
    int m_nLeaderboardEntries;							// number of leaderboard entries we received

    SteamLeaderboard_t m_hSteamLeaderboard;			// handle to the leaderboard we are displaying
    ELeaderboardDataRequest m_eLeaderboardData;		// type of data we are displaying
    bool m_bLoading;								// waiting to receive leaderboard results
    bool m_bIOFailure;								// last attempt to retrieve the leaderboard failed

    TTF_Font* m_fntLeaderboard;                                           // Leaderboard font
    std::vector<UTexture> m_texLeaderboardEntriesRank;                    // Top ten leaderboard entry rank texture
    std::vector<UTexture> m_texLeaderboardEntriesName;                    // Top ten leaderboard entry name texture
    std::vector<UTexture> m_texLeaderboardEntriesScore;                   // Top ten leaderboard entry score texture
    UTexture m_texUserEntryRank, m_texUserEntryName, m_texUserEntryScore; // Users leaderboard entry

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
        m_usersEntry = nullptr;
        m_fntLeaderboard = nullptr;
    }

    // Destructor
    ~STEAM_LeaderboardMenu()
    {
        if (m_usersEntry)
        {
            delete m_usersEntry;
            m_usersEntry = nullptr;
        }

        if (m_fntLeaderboard)
        {
            TTF_CloseFont(m_fntLeaderboard);
            m_fntLeaderboard = nullptr;
        }

        // Free textures
        for (UTexture& e : m_texLeaderboardEntriesRank)
        {
            e.free();
        }
        for (UTexture& e : m_texLeaderboardEntriesName)
        {
            e.free();
        }
        for (UTexture& e : m_texLeaderboardEntriesScore)
        {
            e.free();
        }
        m_texUserEntryRank.free();
        m_texUserEntryName.free();
        m_texUserEntryScore.free();
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
                // printf("Failed to load fastest run leaderboard!\n");
                success = false;
            }

            m_texLongDistanceBoard.initUTexture(a_pRenderer);
            if (!m_texLongDistanceBoard.loadFromFile("assets/longest_distance_leaderboard.png"))
            {
                // printf("Failed to load longest distance leaderboard!\n");
                success = false;
            }

            m_texExitBtn.initUTexture(a_pRenderer);
            if (!m_texExitBtn.loadFromFile("assets/x_button.png"))
            {
                // printf("Failed to load exit button!\n");
                success = false;
            }

            // Init leaderboard font
            m_fntLeaderboard = TTF_OpenFont("assets/font.ttf", 18);
            if (!m_fntLeaderboard)
            {
                // printf("Failed to load leaderboard font, SDL_ttf Error: %s\n", TTF_GetError());
                success = false;
            }

            // Init the leaderboard textures
            for (int i = 0; i < k_nMaxLeaderboardEntries + 1; ++i)
            {
                m_texLeaderboardEntriesRank.push_back(UTexture());
                m_texLeaderboardEntriesRank.back().initUTexture(a_pRenderer);
                m_texLeaderboardEntriesRank.back().initFont(m_fntLeaderboard);

                if (!m_texLeaderboardEntriesRank.back().loadFromRenderedText(" ", BLACK_TEXT))
                {
                    // printf("Failed to load leaderboard entry rank texture!\n");
                    success = false;
                }

                m_texLeaderboardEntriesName.push_back(UTexture());
                m_texLeaderboardEntriesName.back().initUTexture(a_pRenderer);
                m_texLeaderboardEntriesName.back().initFont(m_fntLeaderboard);

                if (!m_texLeaderboardEntriesName.back().loadFromRenderedText(" ", BLACK_TEXT))
                {
                    // printf("Failed to load leaderboard entry name texture!\n");
                    success = false;
                }

                m_texLeaderboardEntriesScore.push_back(UTexture());
                m_texLeaderboardEntriesScore.back().initUTexture(a_pRenderer);
                m_texLeaderboardEntriesScore.back().initFont(m_fntLeaderboard);

                if (!m_texLeaderboardEntriesScore.back().loadFromRenderedText(" ", BLACK_TEXT))
                {
                    // printf("Failed to load leaderboard entry score texture!\n");
                    success = false;
                }
            }

            m_texUserEntryRank = UTexture();
            m_texUserEntryRank.initUTexture(a_pRenderer);
            m_texUserEntryRank.initFont(m_fntLeaderboard);

            if (!m_texUserEntryRank.loadFromRenderedText(" ", BLACK_TEXT))
            {
                // printf("Failed to load user entry rank texture!\n");
                success = false;
            }

            m_texUserEntryName = UTexture();
            m_texUserEntryName.initUTexture(a_pRenderer);
            m_texUserEntryName.initFont(m_fntLeaderboard);

            if (!m_texUserEntryName.loadFromRenderedText(" ", BLACK_TEXT))
            {
                // printf("Failed to load user entry name texture!\n");
                success = false;
            }

            m_texUserEntryScore = UTexture();
            m_texUserEntryScore.initUTexture(a_pRenderer);
            m_texUserEntryScore.initFont(m_fntLeaderboard);

            if (!m_texUserEntryScore.loadFromRenderedText(" ", BLACK_TEXT))
            {
                // printf("Failed to load user entry score texture!\n");
                success = false;
            }
        }
        return success;
    }

    // Menu that shows a leaderboard
    void ShowLeaderboard(SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardData)
    {
        m_hSteamLeaderboard = hLeaderboard;
        m_eLeaderboardData = eLeaderboardData;
        m_bLoading = true;
        m_bIOFailure = false;

        if (hLeaderboard)
        {
            // load the top ten leaderboard data. We only display k_nMaxLeaderboardEntries entries at a time
            if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobal)
            {
                SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, eLeaderboardData, 0, k_nMaxLeaderboardEntries);
                // Register for the async callback
                m_callResultDownloadEntries.Set(hSteamAPICall, this, &STEAM_LeaderboardMenu::OnLeaderboardDownloadedEntries);
            }
            // load the current users
            else if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobalAroundUser)
            {
                SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, eLeaderboardData, -1, 1);
                // Register for the async callback
                m_callResultDownloadEntries.Set(hSteamAPICall, this, &STEAM_LeaderboardMenu::OnLeaderboardDownloadedEntries);
            }
        }

        Rebuild();
    }

    // Render the leaderboard menu
    void Render(bool a_bRenderFastRunLeaderboard) 
    {
        if (a_bRenderFastRunLeaderboard)
        {
            m_texFastRunBoard.render(static_cast<int>((ULib::SCREEN_DIMENSIONS.x - m_texFastRunBoard.getWidth()) / 2.f), static_cast<int>((ULib::SCREEN_DIMENSIONS.y - m_texFastRunBoard.getHeight()) / 2.f));
        }
        else
        {
            m_texLongDistanceBoard.render(static_cast<int>((ULib::SCREEN_DIMENSIONS.x - m_texLongDistanceBoard.getWidth()) / 2.f), static_cast<int>((ULib::SCREEN_DIMENSIONS.y - m_texLongDistanceBoard.getHeight()) / 2.f));
        }

        
        for (int i = 0; i < k_nMaxLeaderboardEntries && i < m_texLeaderboardEntriesName.size(); ++i)
        {
            m_texLeaderboardEntriesRank[i].render(446, 137 + (40 * i));
            m_texLeaderboardEntriesName[i].render(515, 137 + (40 * i));
            m_texLeaderboardEntriesScore[i].render(762, 137 + (40 * i));
        }

        m_texUserEntryRank.render(460, 620);
        m_texUserEntryName.render(520, 620);
        m_texUserEntryScore.render(770, 620);
    }

    // Creates a leaderboard menu
    void Rebuild()
    {
        // If the Leaderboard is loading set the user xor leaderboard entry as loading
        if (!m_hSteamLeaderboard || m_bLoading)
        {
            // Make sure to only erase the leaderboard entries if not requesting just
            // the current user
            if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobal)
            {
                m_leaderboardEntries.clear();
                m_leaderboardEntries.push_back(LeaderboardEntry(ELeaderboardEntryOption::k_ELoading));
                
                for (int i = 0; i < m_leaderboardEntries.size(); ++i)
                {
                    m_texLeaderboardEntriesName[i].loadFromRenderedText(" ", BLACK_TEXT);
                    m_texLeaderboardEntriesRank[i].loadFromRenderedText(" ", BLACK_TEXT);
                    m_texLeaderboardEntriesScore[i].loadFromRenderedText(" ", BLACK_TEXT);
                }
            }

            // Make sure to only erase the user entry if requesting just the current user
            else if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobalAroundUser)
            {
                if (m_usersEntry)
                {
                    delete m_usersEntry;
                    m_usersEntry = nullptr;
                }
                m_usersEntry = new LeaderboardEntry(ELeaderboardEntryOption::k_ELoading);
                m_texUserEntryName.loadFromRenderedText(" ", BLACK_TEXT);
                m_texUserEntryRank.loadFromRenderedText(" ", BLACK_TEXT);
                m_texUserEntryScore.loadFromRenderedText(" ", BLACK_TEXT);
            }
        }

        // If an IO Failure occurs set the user and leaderboard entries as IO failure
        else if (m_bIOFailure)
        {
            m_leaderboardEntries.clear();
            m_leaderboardEntries.push_back(LeaderboardEntry(ELeaderboardEntryOption::k_EIOFail));

            for (int i = 0; i < m_leaderboardEntries.size(); ++i)
            {
                m_texLeaderboardEntriesName[i].loadFromRenderedText(" ", BLACK_TEXT);
                m_texLeaderboardEntriesRank[i].loadFromRenderedText(" ", BLACK_TEXT);
                m_texLeaderboardEntriesScore[i].loadFromRenderedText(" ", BLACK_TEXT);
            }

            if (m_usersEntry)
            {
                delete m_usersEntry;
                m_usersEntry = nullptr;
            }
            m_usersEntry = new LeaderboardEntry(ELeaderboardEntryOption::k_EIOFail);
            m_texUserEntryName.loadFromRenderedText(" ", BLACK_TEXT);
            m_texUserEntryRank.loadFromRenderedText(" ", BLACK_TEXT);
            m_texUserEntryScore.loadFromRenderedText(" ", BLACK_TEXT);
        }

        // Populate the leaderboard entries
        else
        {
            if (m_nLeaderboardEntries == 0)
            {
                // If requesting global scores around the user returns successfully with 0 results
                // the user does not have an entry on the leaderboard
                std::string strText;         
                if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobalAroundUser)
                {
                    if (m_usersEntry)
                    {
                        delete m_usersEntry;
                        m_usersEntry = nullptr;
                    }

                    m_usersEntry = new LeaderboardEntry(ELeaderboardEntryOption::k_ENoScore);
                    m_texUserEntryName.loadFromRenderedText("No score", BLACK_TEXT);
                    m_texUserEntryRank.loadFromRenderedText(" ", BLACK_TEXT);
                    m_texUserEntryScore.loadFromRenderedText(" ", BLACK_TEXT);
                }

                // If not requesting global score around the user
                else if(m_eLeaderboardData == k_ELeaderboardDataRequestGlobal)
                {
                    m_leaderboardEntries.clear();
                    m_leaderboardEntries.push_back(LeaderboardEntry(ELeaderboardEntryOption::k_ENoScore));

                    for (int i = 0; i < m_leaderboardEntries.size(); ++i)
                    {
                        m_texLeaderboardEntriesName[i].loadFromRenderedText(" ", BLACK_TEXT);
                        m_texLeaderboardEntriesRank[i].loadFromRenderedText(" ", BLACK_TEXT);
                        m_texLeaderboardEntriesScore[i].loadFromRenderedText(" ", BLACK_TEXT);
                    }
                }
            }

            // Print the parsed top ten leaderboard data
            else if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobal)
            {
                // clear leaderboard entry textures
                for (int i = 0; i < m_leaderboardEntries.size(); ++i)
                {
                    m_texLeaderboardEntriesName[i].loadFromRenderedText(" ", BLACK_TEXT);
                    m_texLeaderboardEntriesRank[i].loadFromRenderedText(" ", BLACK_TEXT);
                    m_texLeaderboardEntriesScore[i].loadFromRenderedText(" ", BLACK_TEXT);
                }

                for (int i = 0; i < m_leaderboardEntries.size(); ++i)
                {
                    if (m_leaderboardEntries[i].m_eOption == ELeaderboardEntryOption::k_EValid)
                    {
                        // printf("Creating valid leaderboard entry name: %s\n", m_leaderboardEntries[i].m_name.c_str());
                        m_texLeaderboardEntriesName[i].loadFromRenderedText(m_leaderboardEntries[i].m_name, BLACK_TEXT);
                        m_texLeaderboardEntriesRank[i].loadFromRenderedText(std::to_string(m_leaderboardEntries[i].m_nGlobalRank), BLACK_TEXT);
                        m_texLeaderboardEntriesScore[i].loadFromRenderedText(std::to_string(m_leaderboardEntries[i].m_nScore), BLACK_TEXT);
                    }
                }

                // Get the current users leaderboard data
                ShowLeaderboard(m_hSteamLeaderboard, k_ELeaderboardDataRequestGlobalAroundUser);
            }

            // Print the parsed current user leaderboard data
            else if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobalAroundUser)
            {
                if (m_usersEntry->m_eOption == ELeaderboardEntryOption::k_EValid)
                {
                    // printf("Creating valid user Entry name: %s\n", m_usersEntry->m_name.c_str());
                    m_texUserEntryName.loadFromRenderedText(m_usersEntry->m_name, BLACK_TEXT);
                    m_texUserEntryRank.loadFromRenderedText(std::to_string(m_usersEntry->m_nGlobalRank), BLACK_TEXT);
                    m_texUserEntryScore.loadFromRenderedText(std::to_string(m_usersEntry->m_nScore), BLACK_TEXT);
                }
            }
        }
    }

    // Called when SteamUserStats()->DownloadLeaderboardEntries() returns asynchronously
    void OnLeaderboardDownloadedEntries(LeaderboardScoresDownloaded_t* pLeaderboardScoresDownloaded, bool bIOFailure)
    {
        LeaderboardEntry_t tmpLdBdEntry;
        m_bLoading = false;
        m_bIOFailure = bIOFailure;

        // printf("USER NAME: %s\n", SteamFriends()->GetPersonaName());

        // leaderboard entries handle will be invalid once we return from this function. Copy all data now.
        m_nLeaderboardEntries = MIN(pLeaderboardScoresDownloaded->m_cEntryCount, k_nMaxLeaderboardEntries);
        for (int index = 0; index < m_nLeaderboardEntries; index++)
        {
            // Store the downloaded leaderboard entry in tmpLBEntry
            SteamUserStats()->GetDownloadedLeaderboardEntry(pLeaderboardScoresDownloaded->m_hSteamLeaderboardEntries,
                index, &tmpLdBdEntry, NULL, 0);

            // printf("(% d) % s - % d\n", tmpLdBdEntry.m_nGlobalRank, SteamFriends()->GetFriendPersonaName(tmpLdBdEntry.m_steamIDUser), tmpLdBdEntry.m_nScore);

            // Parse the data from tmpLdBdEntry into the appropriate LeaderboardEntry(s)
            if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobal)
            {
                m_leaderboardEntries.push_back(LeaderboardEntry(SteamFriends()->GetFriendPersonaName(tmpLdBdEntry.m_steamIDUser), tmpLdBdEntry.m_nScore, tmpLdBdEntry.m_nGlobalRank));
            }

            // Used to get the users leaderboard entry
            else if (m_eLeaderboardData == k_ELeaderboardDataRequestGlobalAroundUser)
            {
                // Assert m_usersEntry is the nullptr
                if (m_usersEntry)
                {
                    delete m_usersEntry;
                    m_usersEntry = nullptr;
                }

                // Load the users entry
                // printf("%s ?= %s\n", SteamFriends()->GetFriendPersonaName(tmpLdBdEntry.m_steamIDUser), SteamFriends()->GetPersonaName());
                if (strcmp(SteamFriends()->GetPersonaName(), SteamFriends()->GetFriendPersonaName(tmpLdBdEntry.m_steamIDUser)) == 0)
                {
                    m_usersEntry = new LeaderboardEntry(SteamFriends()->GetFriendPersonaName(tmpLdBdEntry.m_steamIDUser), tmpLdBdEntry.m_nScore, tmpLdBdEntry.m_nGlobalRank);
                    break;
                }
            }
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
    bool success = true;

    m_pLeaderboardMenu = new STEAM_LeaderboardMenu();
    if (!m_pLeaderboardMenu->init(a_pRenderer))
    {
        // printf("Failed to load the leaderboard menu!\n");
        success = false;
    }

    if (!m_btnDirectionArrow.init(a_pRenderer, "assets/direction_arrows.png", DIRECTION_BTN_POSITON_1, OPTION_BTN_DIMENSION))
    {
        // printf("Failed to load the direction arrow button!\n");
        success = false;
    }

    ShowFastestRun();

    return success;
}

// Run a frame for the STEAM_Leaderboards
void STEAM_Leaderboards::update()
{
    if (m_btnDirectionArrow.clicked())
    {
        if (m_bRenderFastRunLeaderboard)
        {
            m_bRenderFastRunLeaderboard = false;
            m_btnDirectionArrow.setPosition(DIRECTION_BTN_POSITION_2);
            ShowLongestDistance();
        }
        else
        {
            m_bRenderFastRunLeaderboard = true;
            m_btnDirectionArrow.setPosition(DIRECTION_BTN_POSITON_1);
            ShowFastestRun();
        }
    }
}

// Shows / refreshes leaderboard (depends on current leaderboard status)
void STEAM_Leaderboards::ShowLeaderboardData()
{
    if (m_bRenderFastRunLeaderboard)
    {
        ShowFastestRun();
    }
    else
    {
        ShowLongestDistance();
    }
}

// Shows / refreshes leaderboard (fastest run)
void STEAM_Leaderboards::ShowFastestRun()
{
    // we want to show the top 10 fastest run. To do so, we request global score data beginning at 0
    m_pLeaderboardMenu->ShowLeaderboard(m_hFastestRunLeaderboard, k_ELeaderboardDataRequestGlobal);
}

// Shows / refreshes leaderboard (longest distance)
void STEAM_Leaderboards::ShowLongestDistance()
{
    // we want to show the 10 users around us
    m_pLeaderboardMenu->ShowLeaderboard(m_hLongDistanceLeaderboard, k_ELeaderboardDataRequestGlobal);
}

// Render the leaderboard menu
void STEAM_Leaderboards::render()
{
    m_pLeaderboardMenu->Render(m_bRenderFastRunLeaderboard);
    if (m_bRenderFastRunLeaderboard)
    {
        m_btnDirectionArrow.render(1);
    }
    else
    {
        m_btnDirectionArrow.render(0);
    }
}

// Handle user events, specifically clicking the leaderboard direction button
void STEAM_Leaderboards::handleEvent(SDL_Event& e)
{
    m_btnDirectionArrow.handleEvent(e);
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
        // printf("error\n");
    }

    if (pScoreUploadedResult->m_bScoreChanged)
    {
        // printf("could display new rank");
    }
}