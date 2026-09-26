//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#ifndef FO_WEAPON_NODEGUN_H
#define TF_WEAPON_NODEGUN_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CFONodeGun C_FONodeGun
#endif

//=============================================================================
//
// FO Weapon Node Gun.
//
class CFONodeGun : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CFONodeGun, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CFONodeGun() {}
	~CFONodeGun() {}

	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_NODEGUN; }

private:

	CFONodeGun( const CFONodeGun & ) {}
};

#endif // FO_WEAPON_ASA10_H