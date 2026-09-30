//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 3 (ranged).
//
//=============================================================================
#ifndef FO_WEAPON_CUSTOM3_H
#define FO_WEAPON_CUSTOM3_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CFOCustom3 C_FOCustom3
#endif

//=============================================================================
//
// FO Custom weapon 3.
//
class CFOCustom3 : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CFOCustom3, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CFOCustom3() {}
	~CFOCustom3() {}

	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_CUSTOM3; }

private:

	CFOCustom3( const CFOCustom3 & ) {}
};

#endif // FO_WEAPON_CUSTOM3_H
