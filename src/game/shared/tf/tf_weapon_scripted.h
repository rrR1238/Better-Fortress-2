//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_SYRINGEGUN_H
#define TF_WEAPON_SYRINGEGUN_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFWeaponScripted C_TFWeaponScripted
#endif

// Weapon Flags
// Combine them to create your weapon.
#define SCRIPTED_WEAPON_GENERIC						0			
#define SCRIPTED_WEAPON_BREAKABLE					(1 << 0)									
#define SCRIPTED_WEAPON_CAN_REPAIR					(1 << 1)				
#define SCRIPTED_WEAPON_HAS_THROWABLE_PRIMARY		(1 << 2)	
#define SCRIPTED_WEAPON_HAS_THROWABLE_SECONDARY	    (1 << 3)	
#define SCRIPTED_WEAPON_HAS_CHARGE_BAR				(1 << 4)	
#define SCRIPTED_WEAPON_HAS_KNOCKBACK				(1 << 5)	
#define SCRIPTED_WEAPON_IS_CONSUMABLE				(1 << 6)	
#define SCRIPTED_WEAPON_IS_PASSIVE					(1 << 7)	
#define SCRIPTED_WEAPON_HAS_DEATH_NOTICE			(1 << 8)	//FISH KILL!
#define SCRIPTED_WEAPON_IS_BUILDER					(1 << 9)	
#define SCRIPTED_WEAPON_HAS_SPECIAL_TAUNT			(1 << 10)	
#define SCRIPTED_WEAPON_CAN_DROP					(1 << 11)	
#define SCRIPTED_WEAPON_HAS_ZOOM					(1 << 12)	//Sniperrifles
#define SCRIPTED_WEAPON_CAN_HEADSHOT				(1 << 13)	// 

#define SCRIPTED_WEAPON_LASTFLAG SCRIPTED_WEAPON_HAS_ZOOM

//=============================================================================
//
// TF Weapon Syringe gun.
//
class CTFWeaponScripted : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFWeaponScripted, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFWeaponScripted() {}
	~CTFWeaponScripted() {}

	virtual void Precache( void );

	//Our weapon flags
	int m_bitsWeaponFlags;

	bool FunctionCallBack( const char * iszFunction );

	//Headshot support
	virtual int		GetDamageType( void ) const;
	bool			CanHeadshot( void ) const { return m_bitsWeaponFlags & SCRIPTED_WEAPON_CAN_HEADSHOT; };

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

	virtual bool	Deploy( void );
	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo );

	virtual int GetWeaponProjectileType( void );
	// virtual CBaseEntity *FireProjectile( CTFPlayer *pPlayer );
	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_SCRIPTED; }
	virtual int		GetWeaponFlags( void ) { return m_iWeaponFlags; }

	//virtual void RemoveProjectileAmmo( CTFPlayer *pPlayer );
	//virtual bool HasPrimaryAmmo( void );

private:
	CNetworkVar( int, m_iWeaponFlags );
	CTFWeaponScripted( const CTFWeaponScripted & ) {}
};

#endif // TF_WEAPON_SYRINGEGUN_H
