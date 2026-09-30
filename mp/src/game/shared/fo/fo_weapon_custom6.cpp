//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 6 (ranged).
//
//=============================================================================
#include "cbase.h"
#include "fo_weapon_custom6.h"

//=============================================================================
//
// Weapon Custom6 tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOCustom6, DT_WeaponCustom6 )

BEGIN_NETWORK_TABLE( CFOCustom6, DT_WeaponCustom6 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOCustom6 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_custom6, CFOCustom6 );
PRECACHE_WEAPON_REGISTER( fo_weapon_custom6 );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CFOCustom6 )
END_DATADESC()
#endif
