//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Sentry.io integration for Custom Fortress 2 crash reporting
//
//=============================================================================//

#ifndef CF_SENTRY_H
#define CF_SENTRY_H
#ifdef _WIN32
#pragma once
#endif

// Set to 1 to enable Sentry integration (requires Sentry Native SDK)
// Only enable on Windows for now - Linux would need the sentry-native library to be linked
#ifdef _WIN32
#define USE_SENTRY 1
#else
#define USE_SENTRY 0
#endif

#if USE_SENTRY

namespace SentryIntegration
{
	// Initialize Sentry SDK
	// pszDSN: Your Sentry DSN (Data Source Name) from project settings
	// pszRelease: Version string (e.g., "customfortress2@1.0.0")
	// pszEnvironment: "production", "development", etc.
	void Init( const char *pszDSN, const char *pszRelease, const char *pszEnvironment );
	
	// Shutdown Sentry SDK
	void Shutdown();
	
	// Set user context
	void SetUser( const char *pszUserID, const char *pszUsername = nullptr );
	
	// Add breadcrumb (for tracking user actions before crash)
	void AddBreadcrumb( const char *pszMessage, const char *pszCategory = "default" );
	
	// Set custom tags
	void SetTag( const char *pszKey, const char *pszValue );
	
	// Set extra context data
	void SetContext( const char *pszKey, const char *pszValue );
	
	// Manually capture exception
	void CaptureException( const char *pszMessage );
	
	// Manually capture message
	void CaptureMessage( const char *pszMessage, int nLevel = 1 ); // 0=debug, 1=info, 2=warning, 3=error
	
	// Check if initialized
	bool IsInitialized();
	
	// Flush pending events (forces upload)
	void Flush( int nTimeoutMs = 2000 );
}

#endif // USE_SENTRY

#endif // CF_SENTRY_H
