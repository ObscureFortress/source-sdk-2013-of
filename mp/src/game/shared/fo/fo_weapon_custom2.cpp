//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 2 (ranged).
//
//=============================================================================
#include "cbase.h"
#include "fo_weapon_custom2.h"

//=============================================================================
//
// Weapon Custom2 tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOCustom2, DT_WeaponCustom2 )

BEGIN_NETWORK_TABLE( CFOCustom2, DT_WeaponCustom2 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOCustom2 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_custom2, CFOCustom2 );
PRECACHE_WEAPON_REGISTER( fo_weapon_custom2 );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CFOCustom2 )
END_DATADESC()
#endif
