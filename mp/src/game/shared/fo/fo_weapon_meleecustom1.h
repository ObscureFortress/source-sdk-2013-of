//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 1 (melee).
//
//=============================================================================
#ifndef FO_WEAPON_MELEECUSTOM1_H
#define FO_WEAPON_MELEECUSTOM1_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CFOMeleeCustom1 C_FOMeleeCustom1
#endif

//=============================================================================
//
// FO Custom melee weapon 1.
//
class CFOMeleeCustom1 : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CFOMeleeCustom1, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CFOMeleeCustom1();

	virtual void		PrimaryAttack();
	virtual int			GetWeaponID( void ) const			{ return FO_WEAPON_MELEECUSTOM1; }

private:

	CFOMeleeCustom1( const CFOMeleeCustom1 & ) {}
};

#endif // FO_WEAPON_MELEECUSTOM1_H
