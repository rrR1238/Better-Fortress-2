
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
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_ScrapBall *CTFProjectile_ScrapBall::Create( CBaseEntity *pLauncher, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_ScrapBall *pRocket = static_cast<CTFProjectile_ScrapBall*>( CTFBaseRocket::Create( pLauncher, "tf_projectile_scrapball", vecOrigin, vecAngles, pOwner ) );

	if ( pRocket )
	{
		pRocket->SetScorer( pScorer );
		pRocket->m_iMetalCost = 0; // Initialize metal cost
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
	// Call base class - this will handle the explosion
	BaseClass::RocketTouch( pOther );
}

int CTFProjectile_ScrapBall::GiveMetal( CTFPlayer *pPlayer )
{
	int iMetalToGive = 40;
	int iMetal = pPlayer->GiveAmmo( iMetalToGive, TF_AMMO_METAL, false, kAmmoSource_DispenserOrCart );

	return iMetal;
}

//-----------------------------------------------------------------------------
// Purpose: Add a player to the list of players hit by this projectile
//-----------------------------------------------------------------------------
void CTFProjectile_ScrapBall::AddHitPlayer( CTFPlayer *pPlayer )
{
	if ( !pPlayer )
		return;
		
	// Check if this player was already hit
	if ( HasHitPlayer( pPlayer ) )
		return;
		
	// Add to the list
	m_HitPlayers.AddToTail( pPlayer->entindex() );
}

//-----------------------------------------------------------------------------
// Purpose: Check if a player was already hit by this projectile
//-----------------------------------------------------------------------------
bool CTFProjectile_ScrapBall::HasHitPlayer( CTFPlayer *pPlayer ) const
{
	if ( !pPlayer )
		return false;
		
	int iPlayerIndex = pPlayer->entindex();
	for ( int i = 0; i < m_HitPlayers.Count(); i++ )
	{
		if ( m_HitPlayers[i] == iPlayerIndex )
			return true;
	}
	
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_ScrapBall::Explode( trace_t *pTrace, CBaseEntity *pOther )
{
	if ( ShouldNotDetonate() )
	{
		Destroy( true );
		return;
	}

	// Validate trace
	if ( !pTrace )
	{
		UTIL_Remove( this );
		return;
	}

	// Save this entity as enemy, they will take 100% damage.
	m_hEnemy = pOther;

	// Invisible.
	SetModelName( NULL_STRING );
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;

	// Pull out a bit.
	if ( pTrace->fraction != 1.0 )
	{
		SetAbsOrigin( pTrace->endpos + ( pTrace->plane.normal * 1.0f ) );
	}

	// Play explosion sound and effect.
	Vector vecOrigin = GetAbsOrigin();
	CPVSFilter filter( vecOrigin );
	
	// Halloween Spell Effect Check
	int iHalloweenSpell = 0;
	int iCustomParticleIndex = GetParticleSystemIndex( SCRAPBALL_EXP_PARTICLE );
	item_definition_index_t ownerWeaponDefIndex = INVALID_ITEM_DEF_INDEX;
	// if the owner is a Sentry, Check its owner
	CBaseEntity *pPlayerOwner = GetOwnerPlayer();

	if ( TF_IsHolidayActive( kHoliday_HalloweenOrFullMoon ) )
	{
		CALL_ATTRIB_HOOK_INT_ON_OTHER( pPlayerOwner, iHalloweenSpell, halloween_pumpkin_explosions );
		if ( iHalloweenSpell > 0 )
		{
			iCustomParticleIndex = GetParticleSystemIndex( "halloween_explosion" );
		}
	}

	CTFWeaponBase *pWeapon = dynamic_cast< CTFWeaponBase * >( GetOriginalLauncher() );
	if ( pWeapon && pWeapon->GetAttributeContainer() && pWeapon->GetAttributeContainer()->GetItem() )
	{
		ownerWeaponDefIndex = pWeapon->GetAttributeContainer()->GetItem()->GetItemDefIndex();
	}
	
	int iLargeExplosion = 0;
	CALL_ATTRIB_HOOK_INT_ON_OTHER( pPlayerOwner, iLargeExplosion, use_large_smoke_explosion );
	if ( iLargeExplosion > 0 )
	{
		DispatchParticleEffect( "explosionTrail_seeds_mvm", GetAbsOrigin(), GetAbsAngles() );
		DispatchParticleEffect( "fluidSmokeExpl_ring_mvm", GetAbsOrigin(), GetAbsAngles() );
	}

	int iOtherEntIndex = pOther ? pOther->entindex() : -1;
	TE_TFExplosion( filter, 0.0f, vecOrigin, pTrace->plane.normal, TF_WEAPON_BMMH, iOtherEntIndex, ownerWeaponDefIndex, SPECIAL1, iCustomParticleIndex );

	CSoundEnt::InsertSound ( SOUND_COMBAT, vecOrigin, 1024, 3.0 );

	// Damage enemies only (no self-damage or team damage)
	CBaseEntity *pAttacker = GetOwnerEntity();
	IScorer *pScorerInterface = dynamic_cast<IScorer*>( pAttacker );
	if ( pScorerInterface )
	{
		pAttacker = pScorerInterface->GetScorer();
	}
	else if ( pAttacker && pAttacker->GetOwnerEntity() )
	{
		pAttacker = pAttacker->GetOwnerEntity();
	}

	float flRadius = GetRadius();

	if ( pAttacker )
	{
		CTFPlayer *pAttackerPlayer = ToTFPlayer( pAttacker );
		CTFPlayer *pTarget = ToTFPlayer( GetEnemy() );
		if ( pTarget )
		{
			// Rocket Specialist
			CheckForStunOnImpact( pTarget );

			RecordEnemyPlayerHit( pTarget, true );
		}

		// Apply radius damage to enemies only
		CTakeDamageInfo info( this, pAttacker, GetOriginalLauncher(), vec3_origin, vecOrigin, GetDamage(), GetDamageType(), GetDamageCustom() );
		
		// Manually apply damage to enemies in radius to avoid friendly fire
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBaseEntity *pEntity = UTIL_PlayerByIndex( i );
			if ( !pEntity )
				continue;
				
			// Skip teammates and the attacker
			if ( pAttackerPlayer && pEntity->GetTeamNumber() == pAttackerPlayer->GetTeamNumber() )
				continue;
				
			float flDist = ( pEntity->GetAbsOrigin() - vecOrigin ).Length();
			if ( flDist <= flRadius )
			{
				// Apply damage with falloff
				float flAdjustedDamage = GetDamage() * ( 1.0f - ( flDist / flRadius ) );
				if ( flAdjustedDamage > 0 )
				{
					CTakeDamageInfo enemyInfo( this, pAttacker, GetOriginalLauncher(), vec3_origin, pEntity->GetAbsOrigin(), flAdjustedDamage, GetDamageType(), GetDamageCustom() );
					pEntity->TakeDamage( enemyInfo );
				}
			}
		}
	}

	// Get the Engineer who fired this projectile
	CTFPlayer *pScorer = ToTFPlayer( GetOwnerEntity() );
	
	// Use the stored metal cost, not the attribute
	int iMetalCost = GetMetalCost();
	
	// Scan for ally players in radius and track hits (excluding the Engineer who fired)
	CUtlVector<CTFPlayer*> hitPlayers;
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CTFPlayer *pPlayer = ToTFPlayer( UTIL_PlayerByIndex( i ) );
		if ( !pPlayer || pPlayer->GetTeamNumber() != GetTeamNumber() )
			continue;
		
		// Exclude the Engineer who fired the scrapball
		if ( pPlayer == pScorer )
			continue;
			
		float flDist = ( pPlayer->GetAbsOrigin() - vecOrigin ).Length();
		if ( flDist <= flRadius )
		{
			// Track this player as hit
			AddHitPlayer( pPlayer );
			hitPlayers.AddToTail( pPlayer );
		}
	}
	
	// Split metal among hit players and give them ammo
	int iHitPlayerCount = GetHitPlayerCount();
	if ( iHitPlayerCount > 0 && iMetalCost > 0 )
	{
		int iMetalPerPlayer = iMetalCost / iHitPlayerCount;
		
		FOR_EACH_VEC( hitPlayers, i )
		{
			CTFPlayer *pPlayer = hitPlayers[i];
			if ( pPlayer )
			{
				// Give this player their share of metal
				pPlayer->GiveAmmo( iMetalPerPlayer, TF_AMMO_METAL, false, kAmmoSource_DispenserOrCart );
				
				// Give primary ammo (based on max ammo * 20%)
				int iMaxPrimary = pPlayer->GetMaxAmmo( TF_AMMO_PRIMARY );
				pPlayer->GiveAmmo( iMaxPrimary * 0.2f, TF_AMMO_PRIMARY, true, kAmmoSource_DispenserOrCart );
				
				// Give secondary ammo (based on max ammo * 20%)
				int iMaxSecondary = pPlayer->GetMaxAmmo( TF_AMMO_SECONDARY );
				pPlayer->GiveAmmo( iMaxSecondary * 0.2f, TF_AMMO_SECONDARY, true, kAmmoSource_DispenserOrCart );
			}
		}
		
		// Play an impact sound
		EmitSound( "Weapon_Arrow.ImpactFleshCrossbowHeal" );
	}

	// Scan for buildings to repair/upgrade
	CUtlVector<CBaseObject*> objVector;
	for ( int i = 0; i < IBaseObjectAutoList::AutoList().Count(); ++i )
	{
		CBaseObject* pObj = static_cast<CBaseObject*>( IBaseObjectAutoList::AutoList()[i] );
		if ( !pObj || pObj->GetTeamNumber() != GetTeamNumber() )
			continue;

		float flDist = (pObj->GetAbsOrigin() - vecOrigin).Length();
		if ( flDist <= flRadius )
		{
			objVector.AddToTail( pObj );
		}
	}
	
	int iValidObjCount = objVector.Count();
	if ( iValidObjCount > 0 && iMetalCost > 0 && pScorer )
	{
		FOR_EACH_VEC( objVector, i )
		{
			CBaseObject *pObj = objVector[i];
			if ( !pObj )
				continue;

			bool bRepairHit = false;
			bool bUpgradeHit = false;

			bRepairHit = ( pObj->Command_Repair( pScorer, iMetalCost, 1.f ) > 0 );

			if ( !bRepairHit )
			{
				bUpgradeHit = pObj->CheckUpgradeOnHit( pScorer );
			}
		}
	}

	// Remove the rocket.
	UTIL_Remove( this );

	return;
}

//-----------------------------------------------------------------------------
// Purpose: Scrapball was deflected.
//-----------------------------------------------------------------------------
void CTFProjectile_ScrapBall::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
{
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
