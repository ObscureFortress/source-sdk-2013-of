//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura paddle - a melee weapon that airblasts deflectable
//			projectiles in front of the owner when swung.
//
//=============================================================================

#include "cbase.h"
#include "fo_weapon_paddle.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#include "tf_gamestats.h"
#include "ilagcompensationmanager.h"
#include "util.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================
//
// Weapon Paddle tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOPaddle, DT_FOWeaponPaddle )

BEGIN_NETWORK_TABLE( CFOPaddle, DT_FOWeaponPaddle )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOPaddle )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_paddle, CFOPaddle );
PRECACHE_WEAPON_REGISTER( fo_weapon_paddle );

#ifdef GAME_DLL

// How far in front of the owner the airblast volume is centered, and its half-extents.
#define FO_PADDLE_BLAST_FORWARD		128.0f
#define FO_PADDLE_BLAST_HALF_XY		128.0f
#define FO_PADDLE_BLAST_HALF_Z		64.0f

// How far ahead the owner's aim is traced to decide which way deflected projectiles go.
#define FO_PADDLE_AIM_DISTANCE		2000.0f

#endif // GAME_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPaddle::PrimaryAttack()
{
	if ( !CanAttack() )
		return;

#ifdef GAME_DLL
	Whack();
#else
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	m_flNextSecondaryAttack = m_flNextPrimaryAttack;

	Swing( pPlayer );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Swing the paddle, airblasting any deflectable projectiles in front
//			of the owner.
//-----------------------------------------------------------------------------
#ifdef GAME_DLL
void CFOPaddle::Whack( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;

	m_flNextSecondaryAttack = m_flNextPrimaryAttack;

	pOwner->NoteWeaponFired();
	pOwner->SpeakWeaponFire();
	CTF_GameStats.Event_PlayerFiredWeapon( pOwner, m_bCurrentAttackIsCrit );

	// Move other players back to history positions based on the owner's lag.
	lagcompensation->StartLagCompensation( pOwner, pOwner->GetCurrentCommand() );

	QAngle vAngles = pOwner->EyeAngles();
	Vector vForward;
	AngleVectors( vAngles, &vForward );

	// Box in front of the owner to search for things to blast.
	Vector vecBlastSize( FO_PADDLE_BLAST_HALF_XY, FO_PADDLE_BLAST_HALF_XY, FO_PADDLE_BLAST_HALF_Z );
	Vector vecOrigin = pOwner->Weapon_ShootPosition() + vForward * FO_PADDLE_BLAST_FORWARD;

	CBaseEntity *pList[64];
	int count = UTIL_EntitiesInBox( pList, 64, vecOrigin - vecBlastSize, vecOrigin + vecBlastSize, 0 );

	for ( int i = 0; i < count; i++ )
	{
		CBaseEntity *pEntity = pList[i];
		if ( !pEntity )
			continue;

		if ( !pEntity->IsAlive() )
			continue;

		// Only things that belong to a team (not unassigned/spectator), and not the owner.
		if ( pEntity->GetTeamNumber() < FIRST_GAME_TEAM )
			continue;

		if ( pEntity == pOwner )
			continue;

		if ( !pEntity->IsDeflectable() )
			continue;

		// Characters are never airblasted by this weapon.
		if ( pEntity->MyCombatCharacterPointer() )
			continue;

		// Make sure we can actually see this entity so we don't hit anything through walls.
		Vector vecAirBlast;
		trace_t trWorld;
		UTIL_TraceLine( pOwner->Weapon_ShootPosition(), pEntity->WorldSpaceCenter(), MASK_SOLID, this, COLLISION_GROUP_DEBRIS, &trWorld );
		if ( trWorld.fraction == 1.0f )
		{
			Vector vecOrigin = pEntity->GetAbsOrigin();
			GetProjectileAirblastSetup( GetTFPlayerOwner(), vecOrigin, &vecAirBlast, false );
			AirBlastProjectile( pEntity, pOwner, this, vecAirBlast );
		}
	}

	lagcompensation->FinishLagCompensation( pOwner );

	Swing( pOwner );
}

//-----------------------------------------------------------------------------
// Purpose: Impact sound for something this paddle airblasted.
//-----------------------------------------------------------------------------
void CFOPaddle::OnAirblast( CBaseEntity *pTarget )
{
	if ( pTarget && pTarget->IsPlayer() )
	{
		pTarget->EmitSound( "TFPlayer.AirBlastImpact" );
	}
	else
	{
		pTarget->EmitSound( "Weapon_FlameThrower.AirBurstAttackDeflect" );
	}
}
#else
void CFOPaddle::Whack( void )
{
}
#endif
