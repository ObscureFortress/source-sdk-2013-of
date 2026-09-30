//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 1 (ranged).
//
//=============================================================================
#ifndef FO_WEAPON_CUSTOM1_H
#define FO_WEAPON_CUSTOM1_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CFOCustom1 C_FOCustom1
#endif

//=============================================================================
//
// FO Custom weapon 1.
//
class CFOCustom1 : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CFOCustom1, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CFOCustom1() {}
	~CFOCustom1() {}

	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_CUSTOM1; }

private:

	CFOCustom1( const CFOCustom1 & ) {}
};

#endif // FO_WEAPON_CUSTOM1_H
