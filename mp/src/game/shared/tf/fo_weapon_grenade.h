//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#ifndef FO_WEAPON_GRENADE_H
#define FO_WEAPON_GRENADE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "tf_weaponbase_grenadeproj.h"

// Client specific.
#ifdef CLIENT_DLL
#define CFOGrenade C_FOGrenade
#endif

#define FO_GRENADE_XBOX_CLIP 4

//=============================================================================
//
// FO Weapon Grenade.
//
class CFOGrenade : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CFOGrenade, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CFOGrenade();
	~CFOGrenade();

	virtual void	Spawn( void );
	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_GRENADE; }
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

	CFOGrenade( const CFOGrenade & ) {}
};

#endif // FO_WEAPON_GRENADE_H