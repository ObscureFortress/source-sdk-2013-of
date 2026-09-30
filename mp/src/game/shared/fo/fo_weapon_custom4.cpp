//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 4 (ranged).
//
//=============================================================================
#include "cbase.h"
#include "fo_weapon_custom4.h"

//=============================================================================
//
// Weapon Custom4 tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOCustom4, DT_WeaponCustom4 )

BEGIN_NETWORK_TABLE( CFOCustom4, DT_WeaponCustom4 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOCustom4 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_custom4, CFOCustom4 );
PRECACHE_WEAPON_REGISTER( fo_weapon_custom4 );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CFOCustom4 )
END_DATADESC()
#endif
