//========= Copyright Valve Corporation & BetterFortress, All rights reserved. ============//
//
// Engineer's Bmmh Scrapball
//
//=============================================================================
#include "cbase.h"
#include "tf_weaponbase.h"
#include "tf_projectile_scrapball.h"
#include "tf_player.h"
#include "tf_gamerules.h"
#include "tf_fx.h"
#include "soundent.h"

//=============================================================================
//
// TF Scrapball functions (Server specific).
//
#define SCRAPBALL_MODEL "models/weapons/w_models/w_bmmh_scrapball.mdl"
#define SCRAPBALL_EXP_PARTICLE "rd_robot_explosion"

LINK_ENTITY_TO_CLASS( tf_projectile_scrapball, CTFProjectile_ScrapBall );
PRECACHE_REGISTER( tf_projectile_scrapball );

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_ScrapBall, DT_TFProjectile_ScrapBall )

BEGIN_NETWORK_TABLE( CTFProjectile_ScrapBall, DT_TFProjectile_ScrapBall )
	SendPropBool( SENDINFO( m_bCritical ) ),
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFProjectile_ScrapBall::CTFProjectile_ScrapBall()
{
	m_iMetalCost = 0;
	m_bCritical = false;
	m_bExploded = false;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFProjectile_ScrapBall::~CTFProjectile_ScrapBall()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_ScrapBall *CTFProjectile_ScrapBall::Create( CBaseEntity *pLauncher, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_ScrapBall *pRocket = static_cast<CTFProjectile_ScrapBall*>( CTFBaseRocket::Create( pLauncher, "tf_projectile_scrapball", vecOrigin, vecAngles, pOwner ) );

	if ( pRocket )
	{
		pRocket->SetScorer( pScorer );
		pRocket->m_iMetalCost = 0;
		pRocket->m_bExploded = false;
	}

	return pRocket;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_ScrapBall::Spawn()
{
	SetModel( SCRAPBALL_MODEL );
	BaseClass::Spawn();

	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
	SetGravity( 1.25 );
	
	m_bExploded = false;
	m_iMetalCost = 0;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_ScrapBall::Precache()
{
	int iModel = PrecacheModel( SCRAPBALL_MODEL );
	PrecacheGibsForModel( iModel );
	PrecacheParticleSystem( "critical_rocket_blue" );
	PrecacheParticleSystem( "critical_rocket_red" );
	PrecacheParticleSystem( "rockettrail" );
	PrecacheParticleSystem( "rockettrail_RocketJumper" );
	PrecacheParticleSystem( SCRAPBALL_EXP_PARTICLE );
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_ScrapBall::SetScorer( CBaseEntity *pScorer )
{
	m_Scorer = pScorer;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBasePlayer *CTFProjectile_ScrapBall::GetScorer( void )
{
	return dynamic_cast<CBasePlayer *>( m_Scorer.Get() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFProjectile_ScrapBall::GetDamageType() 
{ 
	int iDmgType = BaseClass::GetDamageType();
	if ( m_bCritical )
	{
		iDmgType |= DMG_CRITICAL;
	}

	return iDmgType;
}

//-----------------------------------------------------------------------------
// Purpose: Scrapball Touch
//-----------------------------------------------------------------------------
void CTFProjectile_ScrapBall::RocketTouch( CBaseEntity *pOther )
{
	// Prevent double-explosion
	if ( m_bExploded )
		return;

	// Verify a correct "other"
	if ( !pOther )
		return;

	// Don't touch triggers or volume contents (unless it's a shield)
	bool bShield = pOther->IsCombatItem() && !InSameTeam( pOther );
	if ( pOther->IsSolidFlagSet( FSOLID_TRIGGER | FSOLID_VOLUME_CONTENTS ) && !bShield )
		return;

	// Handle hitting skybox (disappear)
	const trace_t *pTrace = &CBaseEntity::GetTouchTrace();
	if ( !pTrace )
		return;
		
	if ( pTrace->surface.flags & SURF_SKY )
	{
		UTIL_Remove( this );
		return;
	}

	// Mark as exploded before calling Explode to prevent re-entry
	m_bExploded = true;

	// Explode - exactly like base rocket
	trace_t trace;
	memcpy( &trace, pTrace, sizeof( trace_t ) );
	Explode( &trace, pOther );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_ScrapBall::Explode( trace_t *pTrace, CBaseEntity *pOther )
{
	// Validate trace pointer first
	if ( !pTrace )
	{
		UTIL_Remove( this );
		return;
	}

	if ( ShouldNotDetonate() )
	{
		Destroy( true );
		return;
	}

	// Save this entity as enemy, they will take 100% damage
	m_hEnemy = pOther;

	// Invisible
	SetModelName( NULL_STRING );
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;

	// Pull out a bit
	if ( pTrace->fraction != 1.0 )
	{
		SetAbsOrigin( pTrace->endpos + ( pTrace->plane.normal * 1.0f ) );
	}

	// Play explosion sound and effect
	Vector vecOrigin = GetAbsOrigin();
	CPVSFilter filter( vecOrigin );
	
	int iCustomParticleIndex = GetParticleSystemIndex( SCRAPBALL_EXP_PARTICLE );
	TE_TFExplosion( filter, 0.0f, vecOrigin, pTrace->plane.normal, TF_WEAPON_BMMH, pOther ? pOther->entindex() : -1, INVALID_ITEM_DEF_INDEX, SPECIAL1, iCustomParticleIndex );
	CSoundEnt::InsertSound( SOUND_COMBAT, vecOrigin, 1024, 3.0 );

	// Damage
	CBaseEntity *pAttacker = GetOwnerEntity();
	IScorer *pScorerInterface = dynamic_cast<IScorer*>( pAttacker );
	if ( pScorerInterface )
	{
		pAttacker = pScorerInterface->GetScorer();
	}

	float flRadius = GetRadius();

	if ( pAttacker )
	{
		CTFPlayer *pTarget = ToTFPlayer( GetEnemy() );
		if ( pTarget )
		{
			CheckForStunOnImpact( pTarget );
			RecordEnemyPlayerHit( pTarget, true );
		}

		CTakeDamageInfo info( this, pAttacker, GetOriginalLauncher(), vec3_origin, vecOrigin, GetDamage(), GetDamageType(), GetDamageCustom() );
		CTFRadiusDamageInfo radiusinfo( &info, vecOrigin, flRadius, NULL, 0.0f );
		TFGameRules()->RadiusDamage( radiusinfo );
		
		// Apply blast jump velocity to the attacker (for Quick Fix mirroring)
		CTFPlayer *pTFAttacker = ToTFPlayer( pAttacker );
		if ( pTFAttacker && pTFAttacker == GetEnemy() )
		{
			// Calculate blast force similar to other explosive weapons
			Vector vecForce = pTFAttacker->GetAbsOrigin() - vecOrigin;
			float flDistance = vecForce.NormalizeInPlace();
			
			if ( flDistance > flRadius )
				flDistance = flRadius;
			
			// Calculate falloff
			float flFalloff = 1.0f - ( flDistance / flRadius );
			
			// Apply vertical and horizontal blast force
			vecForce.z = 0.4f; // Lift component
			vecForce.NormalizeInPlace();
			
			float flForce = GetDamage() * 16.0f * flFalloff; // Base blast force
			pTFAttacker->ApplyAbsVelocityImpulse( vecForce * flForce );
		}
	}

	// Remove the rocket
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose: Scrapball was deflected.
//-----------------------------------------------------------------------------
void CTFProjectile_ScrapBall::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
{
	// Reset exploded flag when deflected so it can explode again
	m_bExploded = false;

	CTFPlayer *pTFDeflector = ToTFPlayer( pDeflectedBy );
	if ( !pTFDeflector )
		return;

	ChangeTeam( pTFDeflector->GetTeamNumber() );
	SetLauncher( pTFDeflector->GetActiveWeapon() );

	CTFPlayer* pOldOwner = ToTFPlayer( GetOwnerEntity() );
	SetOwnerEntity( pTFDeflector );

	if ( pOldOwner )
	{
		pOldOwner->SpeakConceptIfAllowed( MP_CONCEPT_DEFLECTED, "projectile:1,victim:1" );
	}

	if ( pTFDeflector->m_Shared.IsCritBoosted() )
	{
		SetCritical( true );
	}

	CTFWeaponBase::SendObjectDeflectedEvent( pTFDeflector, pOldOwner, GetWeaponID(), this );

	IncrementDeflected();
	m_nSkin = ( GetTeamNumber() == TF_TEAM_BLUE ) ? 1 : 0;
}
