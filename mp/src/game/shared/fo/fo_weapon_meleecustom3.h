//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 3 (melee).
//
//=============================================================================
#ifndef FO_WEAPON_MELEECUSTOM3_H
#define FO_WEAPON_MELEECUSTOM3_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CFOMeleeCustom3 C_FOMeleeCustom3
#endif

//=============================================================================
//
// FO Custom melee weapon 3.
//
class CFOMeleeCustom3 : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CFOMeleeCustom3, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CFOMeleeCustom3();

	virtual void		PrimaryAttack();
	virtual int			GetWeaponID( void ) const			{ return FO_WEAPON_MELEECUSTOM3; }

private:

	CFOMeleeCustom3( const CFOMeleeCustom3 & ) {}
};

#endif // FO_WEAPON_MELEECUSTOM3_H
