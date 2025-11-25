//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_fireaxe.h"

//=============================================================================
//
// Weapon FireAxe tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFFireAxe, DT_TFWeaponFireAxe )

BEGIN_NETWORK_TABLE( CTFFireAxe, DT_TFWeaponFireAxe )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFFireAxe )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_fireaxe, CTFFireAxe );
PRECACHE_WEAPON_REGISTER( tf_weapon_fireaxe );

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFFireAxe::GetInitialAfterburnDuration() const 
{ 
	int iAddBurningDamageType = 0;
	CALL_ATTRIB_HOOK_INT( iAddBurningDamageType, set_dmgtype_ignite );
	if ( iAddBurningDamageType )
	{
		return 7.5f;
	}

	return BaseClass::GetInitialAfterburnDuration();
}
#endif
void CTFFireAxe::Precache( void )
{
	PrecacheParticleSystem( "balefulcandle" );
	PrecacheParticleSystem( "drg_3rd_idle" );
	return BaseClass::Precache();
}
bool CTFFireAxe::Holster( CBaseCombatWeapon *pSwitchingTo )
{
#ifdef CLIENT_DLL
	m_bEffectsThinking = false;
#endif

	return BaseClass::Holster( pSwitchingTo );
}
bool CTFFireAxe::Deploy( void )
{
//Certain fireaxes like the Third Degree and Baleful Beacon have special particle effects when out.
#ifdef CLIENT_DLL
	m_ParticleEffect = NULL;
	m_bEffectsThinking = true;
	ClientEffectsThink();
#endif
	return BaseClass::Deploy();
}
#ifdef CLIENT_DLL
void CTFFireAxe::ClientEffectsThink( void )
{
	CTFPlayer* pPlayer = GetTFPlayerOwner();
	int iBeacon = 0;
	int iDRG = 0;
	CALL_ATTRIB_HOOK_INT(iBeacon, lit_candle);
	CALL_ATTRIB_HOOK_INT(iDRG, damage_all_connected);
	if ( iBeacon || iDRG ) {
		if ( !pPlayer )
			return;

		if ( !m_bEffectsThinking )
			return;

		if ( !GetOwner() || GetOwner()->GetActiveWeapon() != this )
			return;

		SetContextThink( &CTFFireAxe::ClientEffectsThink, gpGlobals->curtime + 0.1f, "EFFECTS_THINK" );
		
	}
	if (iBeacon && !m_ParticleEffect) {
	m_ParticleEffect = GetAppropriateWorldOrViewModel()->ParticleProp()->Create( "balefulcandle", PATTACH_POINT_FOLLOW, "candle" );
	}
	if (iDRG && !m_ParticleEffect) {
	m_ParticleEffect = GetAppropriateWorldOrViewModel()->ParticleProp()->Create( "drg_3rd_idle", PATTACH_POINT_FOLLOW, "electrode_0" );
	}
}
#endif
