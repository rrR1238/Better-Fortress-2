//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Engineer's Speed Pad - gives speed boost to players
//
//=============================================================================//

#ifndef TF_OBJ_SPEEDPAD_H
#define TF_OBJ_SPEEDPAD_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_obj.h"

class CTFPlayer;

#define SPEEDPAD_MAX_HEALTH	150

// ------------------------------------------------------------------------ //
// Speed Pad object
// ------------------------------------------------------------------------ //
class CObjectSpeedPad : public CBaseObject
{
	DECLARE_CLASS( CObjectSpeedPad, CBaseObject );

public:
	DECLARE_SERVERCLASS();

	CObjectSpeedPad();

	virtual void	Spawn();
	virtual void	Precache();
	virtual void	SetModel( const char *pModel );
	virtual bool	StartBuilding( CBaseEntity *pBuilder );
	virtual void	OnGoActive( void );
	
	void SpeedPadThink( void );
	void SpeedPadTouch( CBaseEntity *pOther );
	virtual void StartTouch( CBaseEntity *pOther );
	virtual void EndTouch( CBaseEntity *pOther );

	void ApplySpeedBoost( CTFPlayer *pPlayer );

	virtual int		GetBaseHealth( void ) { return SPEEDPAD_MAX_HEALTH; }

protected:
	CNetworkVar( int, m_iTimesUsed );

private:
	DECLARE_DATADESC();

	CHandle<CTFPlayer> m_hLastTouchedPlayer;
	float m_flNextBoostTime;
};

#endif // TF_OBJ_SPEEDPAD_H
