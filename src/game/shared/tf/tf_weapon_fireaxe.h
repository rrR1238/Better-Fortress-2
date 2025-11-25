//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_FIREAXE_H
#define TF_WEAPON_FIREAXE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFFireAxe C_TFFireAxe
#endif

//=============================================================================
//
// BrandingIron class.
//
class CTFFireAxe : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFFireAxe, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFFireAxe() {}
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_FIREAXE; }

#ifdef GAME_DLL
	virtual float GetInitialAfterburnDuration() const OVERRIDE;
#endif

	virtual void Precache( void ) OVERRIDE;
	virtual bool Holster( CBaseCombatWeapon *pSwitchingTo ) OVERRIDE;
	virtual bool Deploy( void ) OVERRIDE;
#ifdef CLIENT_DLL
	void ClientEffectsThink( void );
	bool m_bEffectsThinking;
	HPARTICLEFFECT		m_ParticleEffect;
#endif
private:

	CTFFireAxe( const CTFFireAxe & ) {}
};

#endif // TF_WEAPON_FIREAXE_H
