//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#include "cbase.h"
#include "fo_weapon_nodegun.h"

//=============================================================================
//
// Weapon Node Gun tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FONodeGun, DT_WeaponNodeGun )

BEGIN_NETWORK_TABLE( CFONodeGun, DT_WeaponNodeGun )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFONodeGun )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_nodegun, CFONodeGun );
PRECACHE_WEAPON_REGISTER( fo_weapon_nodegun );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CFONodeGun )
END_DATADESC()
#endif

//=============================================================================
//
// Weapon Node Gun functions.
//