//========= Copyright Valve Corporation & BetterFortress, All rights reserved. ============//
//
// Purpose: Custom Fortress 2 Server Crash Handler Implementation
//         Captures crash information and logs stack traces on Linux dedicated servers
//
//=============================================================================//

#include "cbase.h"
#include "cf_crashhandler_server.h"
#include "filesystem.h"
#include "tier1/strtools.h"

#ifdef POSIX
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <errno.h>
#else
#include <Windows.h>
#include <io.h>
#define fsync _commit
#define fileno _fileno
#endif

#include <time.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
#define MAX_STACK_FRAMES 64
#define CRASH_LOG_DIR "customfortress/crashes"

//-----------------------------------------------------------------------------
// ConVars
//-----------------------------------------------------------------------------
ConVar sv_crashreporting_enabled( "sv_crashreporting_enabled", "1", FCVAR_NONE,
	"Enable server crash reporting and stack trace logging." );

//-----------------------------------------------------------------------------
// Static members
//-----------------------------------------------------------------------------
ServerCrashMetadata_t CServerCrashHandler::s_Metadata;
bool CServerCrashHandler::s_bEnabled = false;
bool CServerCrashHandler::s_bInitialized = false;

#ifdef POSIX
// Store original signal handlers
static struct sigaction s_OldSIGSEGV;
static struct sigaction s_OldSIGABRT;
static struct sigaction s_OldSIGFPE;
static struct sigaction s_OldSIGILL;
static struct sigaction s_OldSIGBUS;

// Flag to prevent recursive crashes
static volatile sig_atomic_t s_bInCrashHandler = 0;
#endif

//-----------------------------------------------------------------------------
// Purpose: Get signal name from signal number
//-----------------------------------------------------------------------------
#ifdef POSIX
static const char* GetSignalName( int nSignal )
{
	switch ( nSignal )
	{
		case SIGSEGV: return "SIGSEGV (Segmentation Fault)";
		case SIGABRT: return "SIGABRT (Abort)";
		case SIGFPE:  return "SIGFPE (Floating Point Exception)";
		case SIGILL:  return "SIGILL (Illegal Instruction)";
		case SIGBUS:  return "SIGBUS (Bus Error)";
		default:      return "Unknown Signal";
	}
}

//-----------------------------------------------------------------------------
// Purpose: Demangle C++ symbol names
//-----------------------------------------------------------------------------
static const char* DemangleSymbol( const char* pszMangledName, char* pszBuffer, size_t nBufferSize )
{
	int nStatus = 0;
	char* pszDemangled = abi::__cxa_demangle( pszMangledName, NULL, NULL, &nStatus );
	
	if ( nStatus == 0 && pszDemangled )
	{
		V_strncpy( pszBuffer, pszDemangled, nBufferSize );
		free( pszDemangled );
		return pszBuffer;
	}
	
	V_strncpy( pszBuffer, pszMangledName, nBufferSize );
	return pszBuffer;
}
#endif

//-----------------------------------------------------------------------------
// Purpose: Initialize crash handler
//-----------------------------------------------------------------------------
// Static path for crash logs - set during Init()
static char s_szCrashLogPath[512] = {0};

void CServerCrashHandler::Init()
{
	if ( s_bInitialized )
	{
		Msg( "[ServerCrashHandler] Already initialized.\n" );
		return;
	}
	
	Msg( "[ServerCrashHandler] Initializing server crash reporting system...\n" );
	
	s_bInitialized = true;
	s_bEnabled = sv_crashreporting_enabled.GetBool();
	
	// Initialize metadata
	V_memset( &s_Metadata, 0, sizeof( s_Metadata ) );
	s_Metadata.bDedicated = true;
	
	// Collect system info
	CollectSystemInfo();
	
	// Set version
	V_snprintf( s_Metadata.szVersion, sizeof( s_Metadata.szVersion ), 
		"Custom Fortress 2 Server" );

#ifdef POSIX
	// Create crash log directory now (mkdir is not signal-safe)
	// First create parent directory, then the crashes subdirectory
	mkdir( "customfortress", 0755 );
	mkdir( CRASH_LOG_DIR, 0755 );
	
	// Get absolute path for signal handler (getcwd may not be signal-safe)
	if ( getcwd( s_szCrashLogPath, sizeof(s_szCrashLogPath) - 64 ) )
	{
		V_strncat( s_szCrashLogPath, "/" CRASH_LOG_DIR, sizeof(s_szCrashLogPath) );
	}
	else
	{
		V_strncpy( s_szCrashLogPath, CRASH_LOG_DIR, sizeof(s_szCrashLogPath) );
	}
	
	if ( s_bEnabled )
	{
		InstallSignalHandlers();
	}
#else
	// Windows: Create directory
	CreateDirectoryA( "customfortress", NULL );
	CreateDirectoryA( CRASH_LOG_DIR, NULL );
	V_strncpy( s_szCrashLogPath, CRASH_LOG_DIR, sizeof(s_szCrashLogPath) );
#endif
	
	Msg( "[ServerCrashHandler] Initialized. Crash logs will be written to %s/\n", s_szCrashLogPath );
}

//-----------------------------------------------------------------------------
// Purpose: Shutdown crash handler
//-----------------------------------------------------------------------------
void CServerCrashHandler::Shutdown()
{
	s_bInitialized = false;
	s_bEnabled = false;
}

//-----------------------------------------------------------------------------
// Purpose: Enable/disable crash reporting
//-----------------------------------------------------------------------------
void CServerCrashHandler::SetEnabled( bool bEnabled )
{
	s_bEnabled = bEnabled;
	sv_crashreporting_enabled.SetValue( bEnabled );
}

bool CServerCrashHandler::IsEnabled()
{
	return s_bEnabled && s_bInitialized;
}

//-----------------------------------------------------------------------------
// Purpose: Set current map
//-----------------------------------------------------------------------------
void CServerCrashHandler::SetCurrentMap( const char *pszMapName )
{
	if ( pszMapName )
	{
		V_strncpy( s_Metadata.szMap, pszMapName, sizeof( s_Metadata.szMap ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set game mode
//-----------------------------------------------------------------------------
void CServerCrashHandler::SetGameMode( const char *pszGameMode )
{
	if ( pszGameMode )
	{
		V_strncpy( s_Metadata.szGameMode, pszGameMode, sizeof( s_Metadata.szGameMode ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set player count
//-----------------------------------------------------------------------------
void CServerCrashHandler::SetPlayerCount( int nPlayers )
{
	s_Metadata.nPlayerCount = nPlayers;
}

//-----------------------------------------------------------------------------
// Purpose: Collect all metadata
// Note: This function may be called from a signal handler, so it must use
//       only async-signal-safe functions when possible
//-----------------------------------------------------------------------------
void CServerCrashHandler::CollectMetadata()
{
	// Generate unique crash ID
	GenerateCrashID();
	
	// Get timestamp - time() and gmtime_r/localtime_r are signal-safe on most systems
	time_t rawtime;
	time( &rawtime );
	struct tm timeinfo;
#ifdef _WIN32
	gmtime_s( &timeinfo, &rawtime );
#else
	gmtime_r( &rawtime, &timeinfo );
#endif
	strftime( s_Metadata.szTimestamp, sizeof( s_Metadata.szTimestamp ), 
		"%Y-%m-%d %H:%M:%S UTC", &timeinfo );
	
	// Get uptime - use static start time to avoid calling Plat_FloatTime in signal handler
	static time_t s_StartTime = 0;
	if ( s_StartTime == 0 )
	{
		time( &s_StartTime );
	}
	s_Metadata.nUptime = (int)( rawtime - s_StartTime );
	
#ifdef POSIX
	// Get memory usage on Linux - getrusage is async-signal-safe
	struct rusage usage;
	if ( getrusage( RUSAGE_SELF, &usage ) == 0 )
	{
		s_Metadata.nMemoryUsageMB = (int)( usage.ru_maxrss / 1024 ); // ru_maxrss is in KB on Linux
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Collect system information
//-----------------------------------------------------------------------------
void CServerCrashHandler::CollectSystemInfo()
{
#ifdef POSIX
	// Get OS info from uname
	struct utsname unameData;
	if ( uname( &unameData ) == 0 )
	{
		V_snprintf( s_Metadata.szOS, sizeof( s_Metadata.szOS ),
			"%s %s %s", unameData.sysname, unameData.release, unameData.machine );
	}
	else
	{
		V_strncpy( s_Metadata.szOS, "Linux (unknown)", sizeof( s_Metadata.szOS ) );
	}
#else
	V_strncpy( s_Metadata.szOS, "Windows Server", sizeof( s_Metadata.szOS ) );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Generate unique crash ID
// Note: Must be signal-safe when called from signal handler
//-----------------------------------------------------------------------------
void CServerCrashHandler::GenerateCrashID()
{
	time_t rawtime;
	time( &rawtime );
	
#ifdef POSIX
	// Use getpid() instead of rand() - signal safe
	V_snprintf( s_Metadata.szCrashID, sizeof( s_Metadata.szCrashID ),
		"srv_%08x_%04x", (unsigned int)rawtime, (unsigned int)getpid() & 0xFFFF );
#else
	V_snprintf( s_Metadata.szCrashID, sizeof( s_Metadata.szCrashID ),
		"srv_%08x_%04x", (unsigned int)rawtime, (unsigned int)GetCurrentProcessId() & 0xFFFF );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Get metadata
//-----------------------------------------------------------------------------
const ServerCrashMetadata_t& CServerCrashHandler::GetMetadata()
{
	return s_Metadata;
}

//-----------------------------------------------------------------------------
// Purpose: Write crash report to file
//-----------------------------------------------------------------------------
bool CServerCrashHandler::WriteCrashReport( const char *pszSignal, void *pFaultAddress )
{
#ifdef POSIX
	// Generate filename with timestamp using signal-safe localtime_r
	time_t tNow = time( NULL );
	struct tm timeinfo;
	localtime_r( &tNow, &timeinfo );
	
	char szFilename[512];
	
	// Use pre-computed absolute path from Init()
	if ( s_szCrashLogPath[0] )
	{
		V_snprintf( szFilename, sizeof(szFilename), 
			"%s/crash_%04d%02d%02d_%02d%02d%02d.log",
			s_szCrashLogPath,
			timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
			timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec );
	}
	else
	{
		// Fallback to relative path
		V_snprintf( szFilename, sizeof(szFilename), 
			"%s/crash_%04d%02d%02d_%02d%02d%02d.log",
			CRASH_LOG_DIR,
			timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
			timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec );
	}

	// Use open() instead of fopen() for better signal safety
	int fd = open( szFilename, O_WRONLY | O_CREAT | O_TRUNC, 0644 );
	if ( fd < 0 )
	{
		// Try /tmp as fallback
		V_snprintf( szFilename, sizeof(szFilename), 
			"/tmp/cf2_crash_%04d%02d%02d_%02d%02d%02d.log",
			timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
			timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec );
		fd = open( szFilename, O_WRONLY | O_CREAT | O_TRUNC, 0644 );
	}

	if ( fd < 0 )
		return false;
	
	// Convert fd to FILE* for easier formatting
	FILE* pFile = fdopen( fd, "w" );
	if ( !pFile )
	{
		close( fd );
		return false;
	}

	// Write header
	fprintf( pFile, "=============================================================\n" );
	fprintf( pFile, "Custom Fortress 2 - Linux Dedicated Server Crash Report\n" );
	fprintf( pFile, "=============================================================\n\n" );

	// Write metadata
	fprintf( pFile, "Crash ID: %s\n", s_Metadata.szCrashID );
	fprintf( pFile, "Version: %s\n", s_Metadata.szVersion );
	fprintf( pFile, "Timestamp: %s\n", s_Metadata.szTimestamp );
	fprintf( pFile, "Map: %s\n", s_Metadata.szMap[0] ? s_Metadata.szMap : "(none)" );
	fprintf( pFile, "Game Mode: %s\n", s_Metadata.szGameMode[0] ? s_Metadata.szGameMode : "(unknown)" );
	fprintf( pFile, "Players: %d\n", s_Metadata.nPlayerCount );
	fprintf( pFile, "Uptime: %d seconds\n", s_Metadata.nUptime );
	fprintf( pFile, "Memory: %d MB\n", s_Metadata.nMemoryUsageMB );
	fprintf( pFile, "OS: %s\n\n", s_Metadata.szOS );

	// Write crash info
	fprintf( pFile, "Signal: %s\n", pszSignal );
	fprintf( pFile, "Fault Address: %p\n\n", pFaultAddress );

	// Get stack trace
	void* pStackFrames[MAX_STACK_FRAMES];
	int nFrameCount = backtrace( pStackFrames, MAX_STACK_FRAMES );

	fprintf( pFile, "Stack Trace (%d frames):\n", nFrameCount );
	fprintf( pFile, "-------------------------------------------------------------\n" );

	// Get symbol names
	char** ppSymbols = backtrace_symbols( pStackFrames, nFrameCount );
	
	if ( ppSymbols )
	{
		char szDemangled[512];
		
		for ( int i = 0; i < nFrameCount; i++ )
		{
			// Try to get more detailed info via dladdr
			Dl_info dlInfo;
			if ( dladdr( pStackFrames[i], &dlInfo ) && dlInfo.dli_sname )
			{
				DemangleSymbol( dlInfo.dli_sname, szDemangled, sizeof(szDemangled) );
				
				// Calculate offset from symbol start
				ptrdiff_t nOffset = (char*)pStackFrames[i] - (char*)dlInfo.dli_saddr;
				
				fprintf( pFile, "#%-2d %p in %s+0x%tx\n", 
					i, 
					pStackFrames[i],
					szDemangled,
					nOffset );
				fprintf( pFile, "    at %s\n", dlInfo.dli_fname ? dlInfo.dli_fname : "unknown" );
			}
			else
			{
				fprintf( pFile, "#%-2d %s\n", i, ppSymbols[i] );
			}
		}
		
		free( ppSymbols );
	}
	else
	{
		fprintf( pFile, "Failed to get symbol names\n" );
	}

	fprintf( pFile, "\n-------------------------------------------------------------\n" );
	
	// Write addr2line commands for easier debugging
	fprintf( pFile, "\nTo get source file and line numbers, run these commands:\n" );
	fprintf( pFile, "(Requires debug symbols - build with -g flag)\n\n" );
	
	for ( int i = 0; i < nFrameCount; i++ )
	{
		Dl_info dlInfo;
		if ( dladdr( pStackFrames[i], &dlInfo ) && dlInfo.dli_fname )
		{
			// Calculate offset within the shared object
			ptrdiff_t nModuleOffset = (char*)pStackFrames[i] - (char*)dlInfo.dli_fbase;
			fprintf( pFile, "addr2line -e %s -f -C 0x%tx\n", dlInfo.dli_fname, nModuleOffset );
		}
	}

	fprintf( pFile, "=============================================================\n" );
	fprintf( pFile, "End of crash report\n" );
	fprintf( pFile, "=============================================================\n" );

	// Ensure data is written to disk
	fflush( pFile );
	fsync( fileno( pFile ) );
	
	fclose( pFile );

	// Also print to stderr
	fprintf( stderr, "\n*** SERVER CRASH: %s at %p ***\n", pszSignal, pFaultAddress );
	fprintf( stderr, "*** Crash log written to: %s ***\n\n", szFilename );
	
	return true;
#else
	// Windows: Use Win32 API instead of fopen (which is blocked by Source Engine)
	(void)pszSignal;
	(void)pFaultAddress;
	
	// Create crashes directory
	CreateDirectoryA( "customfortress", NULL );
	CreateDirectoryA( CRASH_LOG_DIR, NULL );
	
	// Generate filename with timestamp
	time_t tNow = time( NULL );
	struct tm timeinfo;
	gmtime_s( &timeinfo, &tNow );
	
	char szFilename[256];
	V_snprintf( szFilename, sizeof(szFilename), 
		"%s\\crash_%04d%02d%02d_%02d%02d%02d.log",
		CRASH_LOG_DIR,
		timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
		timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec );

	// Use CreateFile instead of fopen
	HANDLE hFile = CreateFileA( szFilename, GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
		return false;

	// Build the crash report as a string
	char szBuffer[4096];
	DWORD dwWritten;
	
	V_snprintf( szBuffer, sizeof(szBuffer),
		"=============================================================\r\n"
		"Custom Fortress 2 - Windows Server Crash Report\r\n"
		"=============================================================\r\n\r\n"
		"Crash ID: %s\r\n"
		"Version: %s\r\n"
		"Timestamp: %s\r\n"
		"Map: %s\r\n"
		"Players: %d\r\n"
		"Uptime: %d seconds\r\n"
		"OS: %s\r\n"
		"\r\nNote: Full stack traces require Windows-specific implementation.\r\n"
		"=============================================================\r\n",
		s_Metadata.szCrashID,
		s_Metadata.szVersion,
		s_Metadata.szTimestamp,
		s_Metadata.szMap[0] ? s_Metadata.szMap : "(none)",
		s_Metadata.nPlayerCount,
		s_Metadata.nUptime,
		s_Metadata.szOS );
	
	WriteFile( hFile, szBuffer, (DWORD)V_strlen(szBuffer), &dwWritten, NULL );
	FlushFileBuffers( hFile );
	CloseHandle( hFile );
	
	return true;
#endif
}

#ifdef POSIX
//-----------------------------------------------------------------------------
// Purpose: Signal handler
//-----------------------------------------------------------------------------
void CServerCrashHandler::SignalHandler( int nSignal, siginfo_t *pInfo, void *pContext )
{
	// Prevent recursive crashes
	if ( s_bInCrashHandler )
	{
		// Already in crash handler, just exit
		_exit( 128 + nSignal );
	}
	s_bInCrashHandler = 1;

	void* pFaultAddress = pInfo ? pInfo->si_addr : NULL;
	const char* pszSignal = GetSignalName( nSignal );
	
	// Store signal info
	V_strncpy( s_Metadata.szSignal, pszSignal, sizeof( s_Metadata.szSignal ) );
	
	// Collect metadata
	CollectMetadata();
	
	// Write crash report
	WriteCrashReport( pszSignal, pFaultAddress );

	// Restore original handler and re-raise signal
	struct sigaction* pOldHandler = NULL;
	switch ( nSignal )
	{
		case SIGSEGV: pOldHandler = &s_OldSIGSEGV; break;
		case SIGABRT: pOldHandler = &s_OldSIGABRT; break;
		case SIGFPE:  pOldHandler = &s_OldSIGFPE;  break;
		case SIGILL:  pOldHandler = &s_OldSIGILL;  break;
		case SIGBUS:  pOldHandler = &s_OldSIGBUS;  break;
	}

	if ( pOldHandler )
	{
		sigaction( nSignal, pOldHandler, NULL );
	}

	// Re-raise to let Breakpad or default handler take over
	raise( nSignal );
}

//-----------------------------------------------------------------------------
// Purpose: Install signal handlers
//-----------------------------------------------------------------------------
void CServerCrashHandler::InstallSignalHandlers()
{
	struct sigaction sa;
	memset( &sa, 0, sizeof(sa) );
	
	sa.sa_sigaction = SignalHandler;
	sa.sa_flags = SA_SIGINFO | SA_RESETHAND;
	sigemptyset( &sa.sa_mask );

	// Install handlers for common crash signals
	sigaction( SIGSEGV, &sa, &s_OldSIGSEGV );
	sigaction( SIGABRT, &sa, &s_OldSIGABRT );
	sigaction( SIGFPE,  &sa, &s_OldSIGFPE );
	sigaction( SIGILL,  &sa, &s_OldSIGILL );
	sigaction( SIGBUS,  &sa, &s_OldSIGBUS );

	Msg( "[ServerCrashHandler] Signal handlers installed for SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS\n" );
}
#endif // POSIX

//-----------------------------------------------------------------------------
// Purpose: Test crash command
//-----------------------------------------------------------------------------
void CServerCrashHandler::TestCrash()
{
	Msg( "[ServerCrashHandler] Triggering test crash...\n" );
	
	// Collect metadata before crash
	CollectMetadata();
	
	Msg( "[ServerCrashHandler] Crash ID: %s\n", s_Metadata.szCrashID );
	Msg( "[ServerCrashHandler] Map: %s\n", s_Metadata.szMap );
	Msg( "[ServerCrashHandler] Crashing now...\n" );
	
	// Trigger a segfault
	int *pNull = NULL;
	*pNull = 42;
}

//-----------------------------------------------------------------------------
// Console command to test crash
//-----------------------------------------------------------------------------
void CC_ServerCrashTest()
{
	CServerCrashHandler::TestCrash();
}

static ConCommand sv_crash_test( "sv_crash_test", CC_ServerCrashTest, 
	"Test server crash handler by triggering an intentional crash.", FCVAR_CHEAT );
