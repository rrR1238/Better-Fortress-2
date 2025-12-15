//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client-side Speed Pad
//
//=============================================================================//

#ifndef C_OBJ_SPEEDPAD_H
#define C_OBJ_SPEEDPAD_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseobject.h"

class C_ObjectSpeedPad : public C_BaseObject
{
	DECLARE_CLASS( C_ObjectSpeedPad, C_BaseObject );
public:
	DECLARE_CLIENTCLASS();

	C_ObjectSpeedPad();

	virtual void OnDataChanged( DataUpdateType_t updateType );
	virtual void ClientThink( void );

	int GetTimesUsed( void ) { return m_iTimesUsed; }

private:
	int m_iTimesUsed;

private:
	C_ObjectSpeedPad( const C_ObjectSpeedPad & ); // not defined, not accessible
};

#endif	//C_OBJ_SPEEDPAD_H
