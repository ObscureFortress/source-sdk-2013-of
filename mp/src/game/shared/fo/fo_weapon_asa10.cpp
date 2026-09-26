//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#include "cbase.h"
#include "fo_weapon_asa10.h"

//=============================================================================
//
// Weapon ASA10 tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOASA10, DT_WeaponASA10 )

BEGIN_NETWORK_TABLE( CFOASA10, DT_WeaponASA10 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOASA10 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_asa10, CFOASA10 );
PRECACHE_WEAPON_REGISTER( fo_weapon_asa10 );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CFOASA10 )
END_DATADESC()
#endif

//=============================================================================
//
// Weapon ASA10 functions.
//