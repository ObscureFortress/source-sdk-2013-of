//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 3 (melee).
//
//=============================================================================

#include "cbase.h"
#include "fo_weapon_meleecustom3.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon MeleeCustom3 tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOMeleeCustom3, DT_FOWeaponMeleeCustom3 )

BEGIN_NETWORK_TABLE( CFOMeleeCustom3, DT_FOWeaponMeleeCustom3 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOMeleeCustom3 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_meleecustom3, CFOMeleeCustom3 );
PRECACHE_WEAPON_REGISTER( fo_weapon_meleecustom3 );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CFOMeleeCustom3::CFOMeleeCustom3()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFOMeleeCustom3::PrimaryAttack()
{
	if ( !CanAttack() )
		return;

	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	m_flNextSecondaryAttack = m_flNextPrimaryAttack;

	Swing( pPlayer );
}
