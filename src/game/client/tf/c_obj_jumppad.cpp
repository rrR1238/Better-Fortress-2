//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client-side Jump Pad
//
//=============================================================================//
#include "cbase.h"
#include "c_obj_jumppad.h"
#include "c_tf_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_ObjectJumpPad, DT_ObjectJumpPad, CObjectJumpPad )
	RecvPropInt( RECVINFO( m_iTimesUsed ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_ObjectJumpPad::C_ObjectJumpPad()
{
	m_iTimesUsed = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectJumpPad::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectJumpPad::ClientThink( void )
{
	BaseClass::ClientThink();
}
