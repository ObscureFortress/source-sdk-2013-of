//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura custom weapon slot 1 (melee).
//
//=============================================================================

#include "cbase.h"
#include "fo_weapon_meleecustom1.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon MeleeCustom1 tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOMeleeCustom1, DT_FOWeaponMeleeCustom1 )

BEGIN_NETWORK_TABLE( CFOMeleeCustom1, DT_FOWeaponMeleeCustom1 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOMeleeCustom1 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_meleecustom1, CFOMeleeCustom1 );
PRECACHE_WEAPON_REGISTER( fo_weapon_meleecustom1 );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CFOMeleeCustom1::CFOMeleeCustom1()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFOMeleeCustom1::PrimaryAttack()
{
	if ( !CanAttack() )
		return;

	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

	Swing( pPlayer );
}
