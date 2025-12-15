//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client-side Speed Pad
//
//=============================================================================//
#include "cbase.h"
#include "c_obj_speedpad.h"
#include "c_tf_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_ObjectSpeedPad, DT_ObjectSpeedPad, CObjectSpeedPad )
	RecvPropInt( RECVINFO( m_iTimesUsed ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_ObjectSpeedPad::C_ObjectSpeedPad()
{
	m_iTimesUsed = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectSpeedPad::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectSpeedPad::ClientThink( void )
{
	BaseClass::ClientThink();
}
