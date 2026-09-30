//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 5 (ranged).
//
//=============================================================================
#include "cbase.h"
#include "fo_weapon_custom5.h"

//=============================================================================
//
// Weapon Custom5 tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOCustom5, DT_WeaponCustom5 )

BEGIN_NETWORK_TABLE( CFOCustom5, DT_WeaponCustom5 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOCustom5 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_custom5, CFOCustom5 );
PRECACHE_WEAPON_REGISTER( fo_weapon_custom5 );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CFOCustom5 )
END_DATADESC()
#endif
