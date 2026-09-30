//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 2 (ranged).
//
//=============================================================================
#ifndef FO_WEAPON_CUSTOM2_H
#define FO_WEAPON_CUSTOM2_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CFOCustom2 C_FOCustom2
#endif

//=============================================================================
//
// FO Custom weapon 2.
//
class CFOCustom2 : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CFOCustom2, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CFOCustom2() {}
	~CFOCustom2() {}

	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_CUSTOM2; }

private:

	CFOCustom2( const CFOCustom2 & ) {}
};

#endif // FO_WEAPON_CUSTOM2_H
