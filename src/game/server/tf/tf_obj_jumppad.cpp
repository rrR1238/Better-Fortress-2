//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Engineer's Jump Pad
//
//=============================================================================//
#include "cbase.h"

#include "tf_obj_jumppad.h"
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

#define JUMPPAD_MODEL_PLACEMENT		"models/buildables/teleporter_blueprint_exit.mdl"
#define JUMPPAD_MODEL_BUILDING		"models/buildables/teleporter.mdl"
#define JUMPPAD_MODEL_LIGHT			"models/buildables/teleporter_light.mdl"

#define JUMPPAD_MINS				Vector( -24, -24, 0)
#define JUMPPAD_MAXS				Vector( 24, 24, 12)	

#define JUMPPAD_THINK_CONTEXT		"JumpPadContext"

// Vertical jump velocity per level
static float g_flJumpPadVelocity[4] = { 0.0f, 400.0f, 600.0f, 900.0f };
// Cooldown time per level
static float g_flJumpPadCooldown[4] = { 0.0f, 8.0f, 6.0f, 3.0f };

IMPLEMENT_SERVERCLASS_ST( CObjectJumpPad, DT_ObjectJumpPad )
	SendPropInt( SENDINFO(m_iTimesUsed), 10, SPROP_UNSIGNED ),
END_SEND_TABLE()

BEGIN_DATADESC( CObjectJumpPad )
	DEFINE_THINKFUNC( JumpPadThink ),
	DEFINE_ENTITYFUNC( JumpPadTouch ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( obj_jumppad, CObjectJumpPad );
PRECACHE_REGISTER( obj_jumppad );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectJumpPad::CObjectJumpPad()
{
	SetMaxHealth( JUMPPAD_MAX_HEALTH );
	SetHealth( JUMPPAD_MAX_HEALTH );
	UseClientSideAnimation();
	SetType( OBJ_JUMPPAD );
	m_iTimesUsed = 0;
	m_flNextBoostTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectJumpPad::Spawn()
{
	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_TRIGGER );
	SetModel( JUMPPAD_MODEL_PLACEMENT );
	m_takedamage = DAMAGE_NO;
	m_iUpgradeLevel = 1;

	BaseClass::Spawn();
	
	SetContextThink( &CObjectJumpPad::JumpPadThink, gpGlobals->curtime + 0.1f, JUMPPAD_THINK_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectJumpPad::Precache()
{
	PrecacheModel( JUMPPAD_MODEL_PLACEMENT );
	PrecacheModel( JUMPPAD_MODEL_BUILDING );
	PrecacheModel( JUMPPAD_MODEL_LIGHT );
	
	PrecacheScriptSound( "Building_Teleporter.Receive" );
	
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectJumpPad::SetModel( const char *pModel )
{
	BaseClass::SetModel( pModel );
	UTIL_SetSize( this, JUMPPAD_MINS, JUMPPAD_MAXS );
	CreateBuildPoints();
	ReattachChildren();
}

//-----------------------------------------------------------------------------
// Purpose: Start building the object
//-----------------------------------------------------------------------------
bool CObjectJumpPad::StartBuilding( CBaseEntity *pBuilder )
{
	SetModel( JUMPPAD_MODEL_BUILDING );
	return BaseClass::StartBuilding( pBuilder );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectJumpPad::OnGoActive( void )
{
	BaseClass::OnGoActive();
	
	SetModel( JUMPPAD_MODEL_LIGHT );
	SetTouch( &CObjectJumpPad::JumpPadTouch );
	
	SetContextThink( &CObjectJumpPad::JumpPadThink, gpGlobals->curtime + 0.1f, JUMPPAD_THINK_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectJumpPad::JumpPadThink( void )
{
	SetContextThink( &CObjectJumpPad::JumpPadThink, gpGlobals->curtime + 0.1f, JUMPPAD_THINK_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CObjectJumpPad::StartTouch( CBaseEntity *pOther )
{
	BaseClass::StartTouch( pOther );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CObjectJumpPad::EndTouch( CBaseEntity *pOther )
{
	BaseClass::EndTouch( pOther );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CObjectJumpPad::JumpPadTouch( CBaseEntity *pOther )
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

	// Apply jump boost
	ApplyJumpBoost( pPlayer );
	
	int iLevel = GetUpgradeLevel();
	if ( iLevel < 1 || iLevel > 3 )
		iLevel = 1;
	
	m_flNextBoostTime = gpGlobals->curtime + g_flJumpPadCooldown[iLevel];
}

//-----------------------------------------------------------------------------
// Purpose: Apply jump boost to player
//-----------------------------------------------------------------------------
void CObjectJumpPad::ApplyJumpBoost( CTFPlayer *pPlayer )
{
	if ( !pPlayer )
		return;

	int iLevel = GetUpgradeLevel();
	if ( iLevel < 1 || iLevel > 3 )
		iLevel = 1;
	
	float flVelocity = g_flJumpPadVelocity[iLevel];

	// Apply upward velocity impulse
	Vector vecImpulse( 0, 0, flVelocity );
	pPlayer->ApplyAbsVelocityImpulse( vecImpulse );

	// Also set the base velocity Z to ensure the boost is applied
	Vector vecBaseVelocity = pPlayer->GetBaseVelocity();
	vecBaseVelocity.z = flVelocity;
	pPlayer->SetBaseVelocity( vecBaseVelocity );

	// Remove fall damage for a short time
	pPlayer->m_Shared.AddCond( TF_COND_PARACHUTE_DEPLOYED, 2.0f );

	Vector origin = GetAbsOrigin();
	CPVSFilter filter( origin );

	int iTeam = GetTeamNumber();
	switch( iTeam )
	{
	case TF_TEAM_RED:
		TE_TFParticleEffect( filter, 0.0, "teleportedin_red", origin, vec3_angle );
		break;
	case TF_TEAM_BLUE:
		TE_TFParticleEffect( filter, 0.0, "teleportedin_blue", origin, vec3_angle );
		break;
	default:
		break;
	}

	EmitSound( "Building_Teleporter.Receive" );

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
