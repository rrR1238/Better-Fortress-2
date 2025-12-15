//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Engineer's Speed Pad
//
//=============================================================================//
#include "cbase.h"

#include "tf_obj_speedpad.h"
#include "engine/IEngineSound.h"
#include "tf_player.h"
#include "tf_team.h"
#include "tf_gamerules.h"
#include "world.h"
#include "particle_parse.h"
#include "tf_gamestats.h"
#include "tf_fx.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SPEEDPAD_MODEL_PLACEMENT	"models/buildables/teleporter_blueprint_enter.mdl"
#define SPEEDPAD_MODEL_BUILDING		"models/buildables/teleporter.mdl"
#define SPEEDPAD_MODEL_LIGHT		"models/buildables/teleporter_light.mdl"

#define SPEEDPAD_MINS				Vector( -24, -24, 0)
#define SPEEDPAD_MAXS				Vector( 24, 24, 12)	

#define SPEEDPAD_THINK_CONTEXT		"SpeedPadContext"

// Speed boost duration per level in seconds
static float g_flSpeedPadDuration[4] = { 0.0f, 5.0f, 8.0f, 10.0f };
// Cooldown time per level
static float g_flSpeedPadCooldown[4] = { 0.0f, 8.0f, 6.0f, 3.0f };

IMPLEMENT_SERVERCLASS_ST( CObjectSpeedPad, DT_ObjectSpeedPad )
	SendPropInt( SENDINFO(m_iTimesUsed), 10, SPROP_UNSIGNED ),
END_SEND_TABLE()

BEGIN_DATADESC( CObjectSpeedPad )
	DEFINE_THINKFUNC( SpeedPadThink ),
	DEFINE_ENTITYFUNC( SpeedPadTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( obj_speedpad, CObjectSpeedPad );
PRECACHE_REGISTER( obj_speedpad );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectSpeedPad::CObjectSpeedPad()
{
	SetMaxHealth( SPEEDPAD_MAX_HEALTH );
	SetHealth( SPEEDPAD_MAX_HEALTH );
	UseClientSideAnimation();
	SetType( OBJ_SPEEDPAD );
	m_iTimesUsed = 0;
	m_flNextBoostTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSpeedPad::Spawn()
{
	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_TRIGGER );
	SetModel( SPEEDPAD_MODEL_PLACEMENT );
	m_takedamage = DAMAGE_NO;
	m_iUpgradeLevel = 1;

	BaseClass::Spawn();
	
	SetContextThink( &CObjectSpeedPad::SpeedPadThink, gpGlobals->curtime + 0.1f, SPEEDPAD_THINK_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSpeedPad::Precache()
{
	PrecacheModel( SPEEDPAD_MODEL_PLACEMENT );
	PrecacheModel( SPEEDPAD_MODEL_BUILDING );
	PrecacheModel( SPEEDPAD_MODEL_LIGHT );
	
	PrecacheScriptSound( "Building_Teleporter.Send" );
	
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSpeedPad::SetModel( const char *pModel )
{
	BaseClass::SetModel( pModel );
	UTIL_SetSize( this, SPEEDPAD_MINS, SPEEDPAD_MAXS );
	CreateBuildPoints();
	ReattachChildren();
}

//-----------------------------------------------------------------------------
// Purpose: Start building the object
//-----------------------------------------------------------------------------
bool CObjectSpeedPad::StartBuilding( CBaseEntity *pBuilder )
{
	SetModel( SPEEDPAD_MODEL_BUILDING );
	return BaseClass::StartBuilding( pBuilder );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSpeedPad::OnGoActive( void )
{
	BaseClass::OnGoActive();
	
	SetModel( SPEEDPAD_MODEL_LIGHT );
	SetTouch( &CObjectSpeedPad::SpeedPadTouch );
	
	SetContextThink( &CObjectSpeedPad::SpeedPadThink, gpGlobals->curtime + 0.1f, SPEEDPAD_THINK_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSpeedPad::SpeedPadThink( void )
{
	SetContextThink( &CObjectSpeedPad::SpeedPadThink, gpGlobals->curtime + 0.1f, SPEEDPAD_THINK_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CObjectSpeedPad::StartTouch( CBaseEntity *pOther )
{
	BaseClass::StartTouch( pOther );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CObjectSpeedPad::EndTouch( CBaseEntity *pOther )
{
	BaseClass::EndTouch( pOther );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CObjectSpeedPad::SpeedPadTouch( CBaseEntity *pOther )
{
	if ( IsDisabled() || IsBuilding() )
		return;

	if ( !pOther->IsPlayer() )
		return;

	CTFPlayer *pPlayer = ToTFPlayer( pOther );
	if ( !pPlayer || !pPlayer->IsAlive() )
		return;

	// Check cooldown
	if ( gpGlobals->curtime < m_flNextBoostTime )
		return;

	// Apply speed boost
	ApplySpeedBoost( pPlayer );
	
	int iLevel = GetUpgradeLevel();
	if ( iLevel < 1 || iLevel > 3 )
		iLevel = 1;
	
	m_flNextBoostTime = gpGlobals->curtime + g_flSpeedPadCooldown[iLevel];
}

//-----------------------------------------------------------------------------
// Purpose: Apply speed boost to player
//-----------------------------------------------------------------------------
void CObjectSpeedPad::ApplySpeedBoost( CTFPlayer *pPlayer )
{
	if ( !pPlayer )
		return;

	int iLevel = GetUpgradeLevel();
	if ( iLevel < 1 || iLevel > 3 )
		iLevel = 1;
	
	float flDuration = g_flSpeedPadDuration[iLevel];

	// Add speed boost condition
	pPlayer->m_Shared.AddCond( TF_COND_SPEED_BOOST, flDuration );

	Vector origin = GetAbsOrigin();
	CPVSFilter filter( origin );

	int iTeam = GetTeamNumber();
	switch( iTeam )
	{
	case TF_TEAM_RED:
		TE_TFParticleEffect( filter, 0.0, "teleported_red", origin, vec3_angle );
		TE_TFParticleEffect( filter, 0.0, "player_sparkles_red", origin, vec3_angle, pPlayer, PATTACH_ABSORIGIN );
		break;
	case TF_TEAM_BLUE:
		TE_TFParticleEffect( filter, 0.0, "teleported_blue", origin, vec3_angle );
		TE_TFParticleEffect( filter, 0.0, "player_sparkles_blue", origin, vec3_angle, pPlayer, PATTACH_ABSORIGIN );
		break;
	default:
		break;
	}

	EmitSound( "Building_Teleporter.Send" );

	m_iTimesUsed++;

	// Award points to builder if different from player
	if ( GetBuilder() && GetBuilder() != pPlayer && 
		 GetBuilder()->GetTeam() == pPlayer->GetTeam() )
	{
		if ( TFGameRules() && 
			 TFGameRules()->GameModeUsesUpgrades() &&
			 TFGameRules()->State_Get() == GR_STATE_RND_RUNNING )
		{
			CTF_GameStats.Event_PlayerAwardBonusPoints( GetBuilder(), pPlayer, 5 );
		}
	}
}
