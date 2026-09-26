//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef FO_WEAPON_HAMMER_H
#define FO_WEAPON_HAMMER_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CFOHammer C_FOHammer
#endif

//=============================================================================
//
// Bat class.
//
class CFOHammer : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CFOHammer, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CFOHammer();

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

	virtual int			GetWeaponID( void ) const			{ return FO_WEAPON_HAMMER; }
	virtual void		Smack(void);

#ifdef GAME_DLL
	void OnFriendlyBuildingHit(CBaseObject *pObject, CTFPlayer *pPlayer);
#endif

private:

	CFOHammer( const CFOHammer & ) {}
};

#endif // FO_WEAPON_HAMMER_H
