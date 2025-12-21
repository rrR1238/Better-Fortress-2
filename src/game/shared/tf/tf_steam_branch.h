//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Steam branch detection utilities
//
//=============================================================================

#ifndef TF_STEAM_BRANCH_H
#define TF_STEAM_BRANCH_H

#ifdef _WIN32
#pragma once
#endif

// Get the current Steam branch name that this build is running on
// Returns "public" for the default branch, or the beta branch name (e.g., "beta")
const char *GetSteamBranchName();

// Check if we're running on a beta branch (anything other than "public")
bool IsRunningOnBetaBranch();

#endif // TF_STEAM_BRANCH_H
