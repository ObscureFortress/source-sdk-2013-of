//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 3 (ranged).
//
//=============================================================================
#include "cbase.h"
#include "fo_weapon_custom3.h"

//=============================================================================
//
// Weapon Custom3 tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOCustom3, DT_WeaponCustom3 )

BEGIN_NETWORK_TABLE( CFOCustom3, DT_WeaponCustom3 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOCustom3 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_custom3, CFOCustom3 );
PRECACHE_WEAPON_REGISTER( fo_weapon_custom3 );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CFOCustom3 )
END_DATADESC()
#endif
