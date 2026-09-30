//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 4 (ranged).
//
//=============================================================================
#ifndef FO_WEAPON_CUSTOM4_H
#define FO_WEAPON_CUSTOM4_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CFOCustom4 C_FOCustom4
#endif

//=============================================================================
//
// FO Custom weapon 4.
//
class CFOCustom4 : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CFOCustom4, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CFOCustom4() {}
	~CFOCustom4() {}

	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_CUSTOM4; }

private:

	CFOCustom4( const CFOCustom4 & ) {}
};

#endif // FO_WEAPON_CUSTOM4_H
