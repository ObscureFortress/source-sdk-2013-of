//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 2 (melee).
//
//=============================================================================
#ifndef FO_WEAPON_MELEECUSTOM2_H
#define FO_WEAPON_MELEECUSTOM2_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CFOMeleeCustom2 C_FOMeleeCustom2
#endif

//=============================================================================
//
// FO Custom melee weapon 2.
//
class CFOMeleeCustom2 : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CFOMeleeCustom2, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CFOMeleeCustom2();

	virtual void		PrimaryAttack();
	virtual int			GetWeaponID( void ) const			{ return FO_WEAPON_MELEECUSTOM2; }

private:

	CFOMeleeCustom2( const CFOMeleeCustom2 & ) {}
};

#endif // FO_WEAPON_MELEECUSTOM2_H
