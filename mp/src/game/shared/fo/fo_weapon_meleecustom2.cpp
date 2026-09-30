//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 2 (melee).
//
//=============================================================================

#include "cbase.h"
#include "fo_weapon_meleecustom2.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon MeleeCustom2 tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOMeleeCustom2, DT_FOWeaponMeleeCustom2 )

BEGIN_NETWORK_TABLE( CFOMeleeCustom2, DT_FOWeaponMeleeCustom2 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOMeleeCustom2 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_meleecustom2, CFOMeleeCustom2 );
PRECACHE_WEAPON_REGISTER( fo_weapon_meleecustom2 );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CFOMeleeCustom2::CFOMeleeCustom2()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFOMeleeCustom2::PrimaryAttack()
{
	if ( !CanAttack() )
		return;

	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

	Swing( pPlayer );
}
