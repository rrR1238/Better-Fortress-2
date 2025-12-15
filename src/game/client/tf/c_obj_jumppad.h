//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client-side Jump Pad
//
//=============================================================================//

#ifndef C_OBJ_JUMPPAD_H
#define C_OBJ_JUMPPAD_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseobject.h"

class C_ObjectJumpPad : public C_BaseObject
{
	DECLARE_CLASS( C_ObjectJumpPad, C_BaseObject );
public:
	DECLARE_CLIENTCLASS();

	C_ObjectJumpPad();

	virtual void OnDataChanged( DataUpdateType_t updateType );
	virtual void ClientThink( void );

	int GetTimesUsed( void ) { return m_iTimesUsed; }

private:
	int m_iTimesUsed;

private:
	C_ObjectJumpPad( const C_ObjectJumpPad & ); // not defined, not accessible
};

#endif	//C_OBJ_JUMPPAD_H
