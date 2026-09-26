//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Concussion Grenade.
//
//=============================================================================//
#include "cbase.h"
#include "tf_weaponbase.h"
#include "tf_gamerules.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"
#include "tf_weapon_grenade_concussion.h"

// Server specific.
#ifdef GAME_DLL
#include "tf_player.h"
#include "items.h"
#include "tf_weaponbase_grenadeproj.h"
#include "tf_fx.h"
#include "soundent.h"
#include "KeyValues.h"

#endif

#define GRENADE_CONCUSSION_TIMER	3.0f			// seconds

//=============================================================================
//
// TF Concussion Grenade tables.
//

IMPLEMENT_NETWORKCLASS_ALIASED( TFGrenadeConcussion, DT_TFGrenadeConcussion )

BEGIN_NETWORK_TABLE( CTFGrenadeConcussion, DT_TFGrenadeConcussion )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFGrenadeConcussion )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_grenade_concussion, CTFGrenadeConcussion );
PRECACHE_WEAPON_REGISTER( tf_weapon_grenade_concussion );

//=============================================================================
//
// TF Concussion Grenade functions.
//

// Server specific.
#ifdef GAME_DLL

BEGIN_DATADESC( CTFGrenadeConcussion )
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFWeaponBaseGrenadeProj *CTFGrenadeConcussion::EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, 
							        AngularImpulse angImpulse, CBasePlayer *pPlayer, float flTime, int iflags )
{
	return CTFGrenadeConcussionProjectile::Create( vecSrc, vecAngles, vecVel, angImpulse, 
		                                pPlayer, GetTFWpnData(), flTime );
}

#endif

//=============================================================================
//
// TF Concussion Grenade Projectile functions (Server specific).
//
#ifdef GAME_DLL

#define GRENADE_MODEL "models/weapons/w_models/w_grenade_conc.mdl"

LINK_ENTITY_TO_CLASS( tf_weapon_grenade_concussion_projectile, CTFGrenadeConcussionProjectile );
PRECACHE_WEAPON_REGISTER( tf_weapon_grenade_concussion_projectile );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFGrenadeConcussionProjectile* CTFGrenadeConcussionProjectile::Create( const Vector &position, const QAngle &angles, 
																const Vector &velocity, const AngularImpulse &angVelocity, 
																CBaseCombatCharacter *pOwner, const CTFWeaponInfo &weaponInfo, float timer, int iFlags )
{
	CTFGrenadeConcussionProjectile *pGrenade = static_cast<CTFGrenadeConcussionProjectile*>( CTFWeaponBaseGrenadeProj::Create( "tf_weapon_grenade_concussion_projectile", position, angles, velocity, angVelocity, pOwner, weaponInfo, timer, iFlags ) );
	if ( pGrenade )
	{
		pGrenade->ApplyLocalAngularVelocityImpulse( angVelocity );
	}

	return pGrenade;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeConcussionProjectile::Spawn()
{
	SetModel( GRENADE_MODEL );

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeConcussionProjectile::Precache()
{
	PrecacheModel( GRENADE_MODEL );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeConcussionProjectile::BounceSound( void )
{
	EmitSound( "Weapon_Grenade_Concussion.Bounce" );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeConcussionProjectile::Detonate()
{
	if ( ShouldNotDetonate() )
	{
		RemoveGrenade();
		return;
	}

	// The trace start/end.
	Vector vecStart = GetAbsOrigin() + Vector( 0.0f, 0.0f, 8.0f );
	Vector vecEnd = vecStart + Vector( 0.0f, 0.0f, -32.0f );

	trace_t	trace;
	UTIL_TraceLine ( vecStart, vecEnd, MASK_SHOT_HULL, this, COLLISION_GROUP_NONE, &trace );

	// Explode (concuss).
	Explode( &trace, DMG_BLAST );

	// Screen shake.
	if ( GetShakeAmplitude() )
	{
		UTIL_ScreenShake( GetAbsOrigin(), GetShakeAmplitude(), 150.0, 1.0, GetShakeRadius(), SHAKE_START );
	}
}

extern ConVar tf_grenade_show_radius;

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeConcussionProjectile::Explode( trace_t *pTrace, int bitsDamageType )
{
// Server specific.
#ifdef GAME_DLL

	// Invisible.
	SetModelName( NULL_STRING );	
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;
	
	// Move the impact point away from the surface a little bit.
	if ( pTrace->fraction != 1.0 )
	{
		SetLocalOrigin( pTrace->endpos + ( pTrace->plane.normal * 0.6 ) );
	}

	// Explosion effect on client
	CPVSFilter filter( GetAbsOrigin() );
	TE_TFExplosion( filter, 0.0f, GetAbsOrigin(), Vector( 0, 0, 1 ), GetWeaponID(), -1 );

	// Explosion sound.
	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), BASEGRENADE_EXPLOSION_VOLUME, 3.0 );

	// Explosion damage, using the thrower's position as the report position.
	CTFPlayer *pPlayer = ToTFPlayer( GetThrower() );
	Vector vecReported = pPlayer ? pPlayer->GetAbsOrigin() : vec3_origin;
	CTakeDamageInfo info( this, pPlayer, GetBlastForce(), GetAbsOrigin(), m_flDamage, bitsDamageType, 0, &vecReported );
	RadiusDamage( info, GetAbsOrigin(), m_DmgRadius, CLASS_NONE, NULL );

	// Concussion.
	CBaseEntity *pEntityList[64];
	int nEntityCount = UTIL_EntitiesInSphere( pEntityList, 64, GetAbsOrigin(), m_DmgRadius, FL_CLIENT );
	for ( int iEntity = 0; iEntity < nEntityCount; ++iEntity )
	{
		CBaseEntity *pEntity = pEntityList[iEntity];
		CTFPlayer *pTestPlayer = ToTFPlayer( pEntity );

		// You can concuss yourself.
		bool bIsThrower = ( pPlayer == pTestPlayer );
		if ( pTestPlayer && ( bIsThrower || !InSameTeam( pTestPlayer ) ) )
		{
			// Scale the effect by distance from the blast.
			Vector vecDir = pTestPlayer->WorldSpaceCenter() - GetAbsOrigin();
			float flDist = vecDir.NormalizeInPlace();
			float flScale = 1.0f - clamp( flDist / m_DmgRadius, 0.0f, 1.0f );

			// Shove them away from the blast and lift them off the ground.
			Vector vecPush = vecDir * ( 500.0f * flScale );
			vecPush.z += 200.0f * flScale;
			pTestPlayer->SetGroundEntity( NULL );
			pTestPlayer->ApplyAbsVelocityImpulse( vecPush );

			// Rattle the view.
			pTestPlayer->ViewPunch( QAngle( random->RandomFloat( -20.0f, 20.0f ) * flScale,
											random->RandomFloat( -20.0f, 20.0f ) * flScale,
											random->RandomFloat( -30.0f, 30.0f ) * flScale ) );
		}
	}

	if ( tf_grenade_show_radius.GetBool() )
	{
		DrawRadius( m_DmgRadius );
	}

	// Explosion decal.
	UTIL_DecalTrace( pTrace, "Scorch" );

	// Reset.
	SetThink( &CBaseGrenade::SUB_Remove );
	SetTouch( NULL );
	AddEffects( EF_NODRAW );
	SetAbsVelocity( vec3_origin );
	SetNextThink( gpGlobals->curtime );
#endif
}

#endif
