//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Custom Fortress 2 Crash Handler - Collects crash data and metadata
//
//=============================================================================//

#ifndef CF_CRASHHANDLER_H
#define CF_CRASHHANDLER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "cf_sentry.h"

// Forward declare Windows types to avoid including windows.h in header
#ifdef _WIN32
struct _EXCEPTION_POINTERS;
#endif

//-----------------------------------------------------------------------------
// Crash Metadata Structure
//-----------------------------------------------------------------------------
struct CrashMetadata_t
{
	char szVersion[64];				// Game version/build number
	char szMap[128];				// Current map name
	char szGameMode[64];			// Game mode (pl, cp, koth, etc)
	char szSteamIDHash[64];			// Hashed Steam ID (for privacy)
	char szTimestamp[64];			// UTC timestamp
	char szCrashID[40];				// Unique crash identifier (UUID)
	
	int nMemoryUsageMB;				// Current memory usage
	int nUptime;					// Game uptime in seconds
	int nPlayerCount;				// Number of players
	int nFPS;						// Current FPS
	
	char szVideoCard[128];			// GPU info
	char szCPU[128];				// CPU info
	char szOS[128];					// OS version
	
	char szLastConsoleLines[4096];	// Last 50 console lines
	
	bool bMultiplayer;				// In multiplayer?
	bool bDedicated;				// Dedicated server?
};

//-----------------------------------------------------------------------------
// Crash Handler Interface
//-----------------------------------------------------------------------------
class CCrashHandler
{
public:
	static void Init( bool bUseSentry = true );
	static void Shutdown();
	
	// Enable/disable crash reporting
	static void SetEnabled( bool bEnabled );
	static bool IsEnabled();
	
	// Set game metadata (called when map loads, etc)
	static void SetCurrentMap( const char *pszMapName );
	static void SetGameMode( const char *pszGameMode );
	static void UpdateMetrics( int nPlayers, int nFPS );
	
	// Called when crash occurs
	static void HandleCrash( unsigned int uStructuredExceptionCode, void *pExceptionInfo );
	
	// Get crash metadata
	static const CrashMetadata_t& GetMetadata();
	
	// Test crash command
	static void TestCrash();
	
private:
	static void CollectMetadata();
	static void CollectSystemInfo();
	static void CollectConsoleHistory();
	static void GenerateCrashID();
	static void HashSteamID( char *pszOutput, int nSize );
	static bool WriteCrashMetadata( const char *pszCrashID );
	static void QueueForUpload( const char *pszMinidumpPath, const char *pszMetadataPath );
	
#ifdef _WIN32
	static long __stdcall CustomExceptionFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );
#endif
	
	static CrashMetadata_t s_Metadata;
	static bool s_bEnabled;
	static bool s_bInitialized;
};

//-----------------------------------------------------------------------------
// ConVars (defined in cpp)
//-----------------------------------------------------------------------------
extern ConVar cf_crashreporting_enabled;
extern ConVar cf_crashreporting_upload;
extern ConVar cf_crashreporting_fullminidump;
extern ConVar cf_crashreporting_server;

#endif // CF_CRASHHANDLER_H
