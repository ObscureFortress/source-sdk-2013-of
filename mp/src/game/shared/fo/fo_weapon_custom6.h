//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 6 (ranged).
//
//=============================================================================
#ifndef FO_WEAPON_CUSTOM6_H
#define FO_WEAPON_CUSTOM6_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CFOCustom6 C_FOCustom6
#endif

//=============================================================================
//
// FO Custom weapon 6.
//
class CFOCustom6 : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CFOCustom6, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CFOCustom6() {}
	~CFOCustom6() {}

	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_CUSTOM6; }

private:

	CFOCustom6( const CFOCustom6 & ) {}
};

#endif // FO_WEAPON_CUSTOM6_H
