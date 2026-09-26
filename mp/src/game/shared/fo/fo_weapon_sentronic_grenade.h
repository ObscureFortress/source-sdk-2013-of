//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#ifndef FO_WEAPON_SENTRONIC_GRENADE_H
#define FO_WEAPON_SENTRONIC_GRENADE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "tf_weaponbase_grenadeproj.h"

// Client specific.
#ifdef CLIENT_DLL
#define CFOSentronicGrenade C_FOSentronicGrenade
#endif

#define TF_GRENADE_LAUNCHER_XBOX_CLIP 4

//=============================================================================
//
// FO Weapon Sentronic Grenade.
//
class CFOSentronicGrenade : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS(CFOSentronicGrenade, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CFOSentronicGrenade();
	~CFOSentronicGrenade();

	virtual void	Spawn( void );
	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_SENTRONIC_GRENADE; }
	virtual void	SecondaryAttack();

	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo );
	virtual bool	Deploy( void );
	virtual void	PrimaryAttack( void );
	virtual void	WeaponIdle( void );
	virtual float	GetProjectileSpeed( void );

	virtual bool	Reload( void );

	virtual int GetMaxClip1( void ) const;
	virtual int GetDefaultClip1( void ) const;

public:

	void LaunchGrenade( void );

private:

	CFOSentronicGrenade( const CFOSentronicGrenade & ) {}
};

#endif // TF_WEAPON_GRENADELAUNCHER_H