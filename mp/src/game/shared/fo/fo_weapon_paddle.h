//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura paddle - a melee weapon that airblasts deflectable
//			projectiles in front of the owner when swung.
//
//=============================================================================

#ifndef FO_WEAPON_PADDLE_H
#define FO_WEAPON_PADDLE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CFOPaddle C_FOPaddle
#endif

//=============================================================================
//
// Paddle class.
//
class CFOPaddle : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CFOPaddle, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CFOPaddle() {}

	virtual void		PrimaryAttack();
	virtual int			GetWeaponID( void ) const			{ return FO_WEAPON_PADDLE; }

	void				Whack( void );

#ifdef GAME_DLL
	virtual void		OnAirblast( CBaseEntity *pTarget );
#endif

private:

	CFOPaddle( const CFOPaddle & ) {}
};

#endif // FO_WEAPON_PADDLE_H
