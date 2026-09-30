//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 1 (ranged).
//
//=============================================================================
#include "cbase.h"
#include "fo_weapon_custom1.h"

//=============================================================================
//
// Weapon Custom1 tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOCustom1, DT_WeaponCustom1 )

BEGIN_NETWORK_TABLE( CFOCustom1, DT_WeaponCustom1 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOCustom1 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_custom1, CFOCustom1 );
PRECACHE_WEAPON_REGISTER( fo_weapon_custom1 );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CFOCustom1 )
END_DATADESC()
#endif
