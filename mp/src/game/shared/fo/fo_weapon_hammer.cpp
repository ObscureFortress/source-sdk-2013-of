//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "fo_weapon_hammer.h"
#include "decals.h"
#include "baseobject_shared.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Bat tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOHammer, DT_FOWeaponHammer )

BEGIN_NETWORK_TABLE( CFOHammer, DT_FOWeaponHammer )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOHammer )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_hammer, CFOHammer );
PRECACHE_WEAPON_REGISTER( fo_weapon_hammer );

//=============================================================================
//
// Weapon Hammer functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CFOHammer::CFOHammer()
{
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CFOHammer::PrimaryAttack()
{
	if (!CanAttack())
		return;

	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if (!pPlayer)
		return;

	m_flNextSecondaryAttack = m_flNextPrimaryAttack;
	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

	Swing(pPlayer);
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CFOHammer::SecondaryAttack()
{
	if (!CanAttack())
		return;

	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if (!pPlayer)
		return;

	m_flNextSecondaryAttack = m_flNextPrimaryAttack;
	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

	DoViewModelAnimation();
	Swing(pPlayer);

	m_iWeaponMode = TF_WEAPON_SECONDARY_MODE;
}

// -----------------------------------------------------------------------------
// Purpose: Crits use the hard swing
// -----------------------------------------------------------------------------
void CFOHammer::DoViewModelAnimation( void )
{
	Activity act = ACT_VM_HITCENTER;
	if ( IsCurrentAttackACrit() )
		act = ACT_VM_SWINGHARD;

	SendWeaponAnim( act );
}

#ifdef GAME_DLL
void CFOHammer::OnFriendlyBuildingHit(CBaseObject *pObject, CTFPlayer *pPlayer)
{
	// Did this object hit do any work? repair or upgrade?
	if (m_iWeaponMode == TF_WEAPON_PRIMARY_MODE)
	{
		bool bUsefulHit = pObject->InputWrenchHit(pPlayer);

		CDisablePredictionFiltering disabler;

		if (bUsefulHit)
		{
			// play success sound
			WeaponSound(SPECIAL1);
		}
		else
		{
			// play failure sound
			WeaponSound(SPECIAL2);
		}
	}
	else
	{
		if (pObject->GetOwner() == pPlayer)
		{
			pObject->DetonateObject();
			WeaponSound(SPECIAL1);
		}
	}
}
#endif

class CTraceFilterIgnorePlayers : public CTraceFilterSimple
{
public:
	// It does have a base, but we'll never network anything below here..
	DECLARE_CLASS(CTraceFilterIgnorePlayers, CTraceFilterSimple);

	CTraceFilterIgnorePlayers(const IHandleEntity *passentity, int collisionGroup)
		: CTraceFilterSimple(passentity, collisionGroup)
	{
	}

	virtual bool ShouldHitEntity(IHandleEntity *pServerEntity, int contentsMask)
	{
		CBaseEntity *pEntity = EntityFromEntityHandle(pServerEntity);
		return pEntity && !pEntity->IsPlayer();
	}
};


void CFOHammer::Smack(void)
{
	// see if we can hit an object with a higher range

	// Get the current player.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if (!pPlayer)
		return;

	if (!CanAttack())
		return;

	// Setup a volume for the melee weapon to be swung - approx size, so all melee behave the same.
	static Vector vecSwingMins(-18, -18, -18);
	static Vector vecSwingMaxs(18, 18, 18);

	// Setup the swing range.
	Vector vecForward;
	AngleVectors(pPlayer->EyeAngles(), &vecForward);
	Vector vecSwingStart = pPlayer->Weapon_ShootPosition();
	Vector vecSwingEnd = vecSwingStart + vecForward * 70;

	// only trace against objects

	// See if we hit anything.
	trace_t trace;

	CTraceFilterIgnorePlayers traceFilter(NULL, COLLISION_GROUP_NONE);
	UTIL_TraceLine(vecSwingStart, vecSwingEnd, MASK_SOLID, &traceFilter, &trace);
	if (trace.fraction >= 1.0)
	{
		UTIL_TraceHull(vecSwingStart, vecSwingEnd, vecSwingMins, vecSwingMaxs, MASK_SOLID, &traceFilter, &trace);
	}

	// We hit, setup the smack.
	if (trace.fraction < 1.0f &&
		trace.m_pEnt &&
		trace.m_pEnt->IsBaseObject() &&
		trace.m_pEnt->GetTeamNumber() == pPlayer->GetTeamNumber())
	{
#ifdef GAME_DLL
		OnFriendlyBuildingHit(dynamic_cast<CBaseObject *>(trace.m_pEnt), pPlayer);
#endif
	}
	else
	{
		// if we cannot, Smack as usual for player hits
		BaseClass::Smack();
	}
}

