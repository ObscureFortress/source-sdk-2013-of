//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 5 (ranged).
//
//=============================================================================
#ifndef FO_WEAPON_CUSTOM5_H
#define FO_WEAPON_CUSTOM5_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CFOCustom5 C_FOCustom5
#endif

//=============================================================================
//
// FO Custom weapon 5.
//
class CFOCustom5 : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CFOCustom5, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CFOCustom5() {}
	~CFOCustom5() {}

	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_CUSTOM5; }

private:

	CFOCustom5( const CFOCustom5 & ) {}
};

#endif // FO_WEAPON_CUSTOM5_H
