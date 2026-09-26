//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#ifndef FO_WEAPON_ASA10_H
#define TF_WEAPON_ASA10_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CFOASA10 C_FOASA10
#endif

//=============================================================================
//
// FO Weapon Automaton Standard Armament Model 10.
//
class CFOASA10 : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CFOASA10, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CFOASA10() {}
	~CFOASA10() {}

	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_ASA10; }

private:

	CFOASA10( const CFOASA10 & ) {}
};

#endif // FO_WEAPON_ASA10_H