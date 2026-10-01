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

//-----------------------------------------------------------------------------
// Purpose: Trace filter that lets traces pass through the owner's teammates.
//-----------------------------------------------------------------------------
class CFOPaddleTraceFilter : public CTraceFilterSimple
{
public:
	CFOPaddleTraceFilter( const IHandleEntity *pPassEntity, int iCollisionGroup, int iIgnoreTeam )
		: CTraceFilterSimple( pPassEntity, iCollisionGroup )
	{
		m_iIgnoreTeam = iIgnoreTeam;
	}

	virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
	{
		CBaseEntity *pEntity = EntityFromEntityHandle( pServerEntity );

		if ( pEntity->IsPlayer() && pEntity->GetTeamNumber() == m_iIgnoreTeam )
		{
			return false;
		}

		return BaseClass::ShouldHitEntity( pServerEntity, contentsMask );
	}

	int m_iIgnoreTeam;
};

//-----------------------------------------------------------------------------
// Purpose: Work out the direction a projectile should be sent in after being
//			airblasted: toward whatever the owner is aiming at.
//-----------------------------------------------------------------------------
static void FO_GetAirblastDirection( CTFPlayer *pPlayer, const Vector &vecProjectilePos, Vector *pvecDeflect, bool bHitTeammates )
{
	Vector vecForward, vecRight, vecUp;
	AngleVectors( pPlayer->EyeAngles(), &vecForward, &vecRight, &vecUp );

	Vector vecShootPos = pPlayer->Weapon_ShootPosition();

	// Estimate an end point and trace to see what's in front of the owner.
	Vector vecEnd = vecShootPos + vecForward * FO_PADDLE_AIM_DISTANCE;

	trace_t tr;
	if ( bHitTeammates )
	{
		CTraceFilterSimple filter( pPlayer, COLLISION_GROUP_NONE );
		UTIL_TraceLine( vecShootPos, vecEnd, MASK_SOLID, &filter, &tr );
	}
	else
	{
		CFOPaddleTraceFilter filter( pPlayer, COLLISION_GROUP_NONE, pPlayer->GetTeamNumber() );
		UTIL_TraceLine( vecShootPos, vecEnd, MASK_SOLID, &filter, &tr );
	}

	if ( r_visualizetraces.GetBool() )
	{
		DebugDrawLine( tr.startpos, tr.endpos, 255, 0, 0, true, -1.0f );
	}

	// If the trace hit something right in front of us, just aim at the far point.
	Vector vecTarget = ( tr.fraction > 0.1 ) ? tr.endpos : vecEnd;

	*pvecDeflect = vecTarget - vecProjectilePos;
	VectorNormalize( *pvecDeflect );
}

//-----------------------------------------------------------------------------
// Purpose: Send a projectile back the way the owner is aiming.
//-----------------------------------------------------------------------------
static void FO_AirblastProjectile( CBaseEntity *pProjectile, CBaseEntity *pOwner, const Vector &vecDir )
{
	if ( !pOwner || !pOwner->IsPlayer() )
		return;

	// Don't deflect our own team's projectiles.
	if ( pProjectile->InSameTeam( pOwner ) )
		return;

	Vector vecDirCopy = vecDir;
	pProjectile->Deflected( pProjectile, vecDirCopy );
	pProjectile->SetOwnerEntity( pOwner );
}

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

	Vector vecForward;
	AngleVectors( pOwner->EyeAngles(), &vecForward );

	// Box in front of the owner to search for things to blast.
	Vector vecCenter = pOwner->Weapon_ShootPosition() + vecForward * FO_PADDLE_BLAST_FORWARD;
	Vector vecMins( vecCenter.x - FO_PADDLE_BLAST_HALF_XY, vecCenter.y - FO_PADDLE_BLAST_HALF_XY, vecCenter.z - FO_PADDLE_BLAST_HALF_Z );
	Vector vecMaxs( vecCenter.x + FO_PADDLE_BLAST_HALF_XY, vecCenter.y + FO_PADDLE_BLAST_HALF_XY, vecCenter.z + FO_PADDLE_BLAST_HALF_Z );

	CBaseEntity *pList[64];
	CFlaggedEntitiesEnum iter( pList, ARRAYSIZE( pList ), 0 );
	int count = UTIL_EntitiesInBox( vecMins, vecMaxs, &iter );

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

		// Needs a clear line from the owner's shoot position.
		trace_t tr;
		UTIL_TraceLine( pOwner->Weapon_ShootPosition(), pEntity->WorldSpaceCenter(), MASK_SOLID, this, COLLISION_GROUP_DEBRIS, &tr );
		if ( tr.fraction != 1.0f )
			continue;

		Vector vecDir;
		FO_GetAirblastDirection( pOwner, pEntity->GetAbsOrigin(), &vecDir, false );
		FO_AirblastProjectile( pEntity, pOwner, vecDir );
	}

	lagcompensation->FinishLagCompensation( pOwner );

	Swing( pOwner );
}

//-----------------------------------------------------------------------------
// Purpose: Impact sound for something this paddle airblasted.
//-----------------------------------------------------------------------------
void CFOPaddle::OnAirblast( CBaseEntity *pTarget )
{
	if ( !pTarget )
		return;

	if ( pTarget->IsPlayer() )
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
