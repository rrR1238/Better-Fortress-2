//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Engineer's Jump Pad - gives jump boost to players
//
//=============================================================================//

#ifndef TF_OBJ_JUMPPAD_H
#define TF_OBJ_JUMPPAD_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_obj.h"

class CTFPlayer;

#define JUMPPAD_MAX_HEALTH	150

// ------------------------------------------------------------------------ //
// Jump Pad object
// ------------------------------------------------------------------------ //
class CObjectJumpPad : public CBaseObject
{
	DECLARE_CLASS( CObjectJumpPad, CBaseObject );

public:
	DECLARE_SERVERCLASS();

	CObjectJumpPad();

	virtual void	Spawn();
	virtual void	Precache();
	virtual void	SetModel( const char *pModel );
	virtual bool	StartBuilding( CBaseEntity *pBuilder );
	virtual void	OnGoActive( void );
	
	void JumpPadThink( void );
	void JumpPadTouch( CBaseEntity *pOther );
	virtual void StartTouch( CBaseEntity *pOther );
	virtual void EndTouch( CBaseEntity *pOther );

	void ApplyJumpBoost( CTFPlayer *pPlayer );

	virtual int		GetBaseHealth( void ) { return JUMPPAD_MAX_HEALTH; }

protected:
	CNetworkVar( int, m_iTimesUsed );

private:
	DECLARE_DATADESC();

	CHandle<CTFPlayer> m_hLastTouchedPlayer;
	float m_flNextBoostTime;
};

#endif // TF_OBJ_JUMPPAD_H
