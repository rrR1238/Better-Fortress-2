//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Async crash report uploader - queues and uploads crashes in background
//
//=============================================================================//

#ifndef CF_CRASHUPLOAD_H
#define CF_CRASHUPLOAD_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier1/utlvector.h"
#include "tier1/utlstring.h"

//-----------------------------------------------------------------------------
// Upload status
//-----------------------------------------------------------------------------
enum CrashUploadStatus_t
{
	CRASH_UPLOAD_PENDING = 0,
	CRASH_UPLOAD_IN_PROGRESS,
	CRASH_UPLOAD_SUCCESS,
	CRASH_UPLOAD_FAILED,
	CRASH_UPLOAD_CANCELLED
};

//-----------------------------------------------------------------------------
// Upload queue entry
//-----------------------------------------------------------------------------
struct CrashUploadEntry_t
{
	CUtlString strCrashID;
	CUtlString strMinidumpPath;
	CUtlString strMetadataPath;
	CrashUploadStatus_t status;
	int nRetryCount;
	float flNextRetryTime;
};

//-----------------------------------------------------------------------------
// Crash Upload Manager
//-----------------------------------------------------------------------------
class CCrashUploadManager
{
public:
	static void Init();
	static void Shutdown();
	static void Think(); // Call this periodically (every frame or so)
	
	// Queue a crash for upload
	static void QueueCrash( const char *pszCrashID, const char *pszMinidumpPath, const char *pszMetadataPath );
	
	// Get upload statistics
	static int GetPendingCount();
	static int GetSuccessCount();
	static int GetFailedCount();
	
	// Manual controls
	static void RetryFailed();
	static void ClearQueue();
	static void CancelAll();
	
private:
	static void LoadQueue();
	static void SaveQueue();
	static void ProcessQueue();
	static bool UploadCrash( CrashUploadEntry_t &entry );
	static bool SendHTTPPost( const char *pszURL, const char *pszMinidumpPath, const char *pszMetadataPath, CUtlString &outResponse );
	static void RemoveOldCrashes();
	
	static CUtlVector<CrashUploadEntry_t> s_Queue;
	static bool s_bInitialized;
	static bool s_bProcessing;
	static float s_flNextProcessTime;
	static int s_nSuccessCount;
	static int s_nFailedCount;
};

#endif // CF_CRASHUPLOAD_H
