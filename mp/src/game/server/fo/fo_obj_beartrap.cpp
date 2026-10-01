//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Saptrap's Beartrap
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include "fo_obj_beartrap.h"
#include "engine/IEngineSound.h"
#include "tf_player.h"
#include "tf_team.h"
#include "world.h"
#include "explode.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define BEARTRAP_MODEL		"models/buildables/beartrap_light.mdl"

#define BEARTRAP_MINS		Vector( -24, -24, 0 )
#define BEARTRAP_MAXS		Vector( 24, 24, 12 )

#define BEARTRAP_MAX_HEALTH		25

// What the builder gets when an enemy steps on the trap.
#define BEARTRAP_ENERGY_REWARD		30.0f
#define BEARTRAP_HEALTH_REWARD		60.0f
#define BEARTRAP_TRIGGER_DAMAGE		60.0f

IMPLEMENT_SERVERCLASS_ST( CObjectBeartrap, DT_ObjectBeartrap )
	SendPropInt( SENDINFO( m_iState ), 5 ),
END_SEND_TABLE()

BEGIN_DATADESC( CObjectBeartrap )
END_DATADESC()

LINK_ENTITY_TO_CLASS( obj_beartrap, CObjectBeartrap );
PRECACHE_REGISTER( obj_beartrap );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectBeartrap::CObjectBeartrap()
{
	m_iState = 0;

	SetMaxHealth( BEARTRAP_MAX_HEALTH );
	m_iHealth = BEARTRAP_MAX_HEALTH;

	UseClientSideAnimation();

	SetType( OBJ_BEARTRAP );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectBeartrap::Spawn()
{
	SetModel( BEARTRAP_MODEL );
	SetSolid( SOLID_BBOX );

	m_takedamage = DAMAGE_YES;
	m_iState = 0;
	m_flLastStateChangeTime = gpGlobals->curtime;
	m_flNextEnemyTouchHint = gpGlobals->curtime;

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectBeartrap::Precache()
{
	BaseClass::Precache();

	int iModelIndex = PrecacheModel( BEARTRAP_MODEL );
	PrecacheGibsForModel( iModelIndex );

	PrecacheScriptSound( "Building_Teleporter.Ready" );
	PrecacheScriptSound( "Building_Teleporter.Send" );
	PrecacheScriptSound( "Building_Teleporter.Receive" );
	PrecacheScriptSound( "Building_Teleporter.Spin" );
}

//-----------------------------------------------------------------------------
// Purpose: Start building the object
//-----------------------------------------------------------------------------
bool CObjectBeartrap::StartBuilding( CBaseEntity *pBuilder )
{
	SetModel( BEARTRAP_MODEL );

	return BaseClass::StartBuilding( pBuilder );
}

//-----------------------------------------------------------------------------
// Purpose: Finished building
//-----------------------------------------------------------------------------
void CObjectBeartrap::FinishedBuilding( void )
{
	BaseClass::FinishedBuilding();

	SetActivity( ACT_OBJ_RUNNING );
	SetPlaybackRate( 0.0f );
}

//-----------------------------------------------------------------------------
// Purpose: Arm the trap.
//-----------------------------------------------------------------------------
void CObjectBeartrap::OnGoActive( void )
{
	if ( !GetBuilder() )
		return;

	SetModel( BEARTRAP_MODEL );
	SetActivity( ACT_OBJ_IDLE );

	SetTouch( &CObjectBeartrap::BeartrapTouch );
	SetState( 1 );

	BaseClass::OnGoActive();

	SetPlaybackRate( 0.0f );
	m_flLastStateChangeTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Sprung by an enemy player: reward the builder, hurt the victim,
//			and use the trap up.
//-----------------------------------------------------------------------------
void CObjectBeartrap::BeartrapTouch( CBaseEntity *pOther )
{
	if ( IsDisabled() )
		return;

	if ( !pOther->IsPlayer() )
		return;

	CBasePlayer *pVictim = ToBasePlayer( pOther );

	CTFPlayer *pBuilder = GetBuilder();
	if ( !pBuilder || pBuilder == pOther )
		return;

	if ( pBuilder->GetTeamNumber() == pOther->GetTeamNumber() )
		return;

	pBuilder->m_Shared.SetSaptrapEnergyMeter( pBuilder->m_Shared.GetSaptrapEnergyMeter() + BEARTRAP_ENERGY_REWARD );
	pBuilder->TakeHealth( BEARTRAP_HEALTH_REWARD, DMG_IGNORE_MAXHEALTH );

	CTakeDamageInfo info( this, pBuilder, BEARTRAP_TRIGGER_DAMAGE, DMG_SLASH | DMG_PREVENT_PHYSICS_FORCE, 0 );
	pVictim->TakeDamage( info );

	DetonateObject();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectBeartrap::SetState( int state )
{
	if ( state != m_iState )
	{
		m_iState = state;
		m_flLastStateChangeTime = gpGlobals->curtime;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectBeartrap::SetModel( const char *pModel )
{
	BaseClass::SetModel( pModel );
	UTIL_SetSize( this, BEARTRAP_MINS, BEARTRAP_MAXS );
}

//-----------------------------------------------------------------------------
// Purpose: The builder shooting their own trap disarms it.
//-----------------------------------------------------------------------------
int CObjectBeartrap::OnTakeDamage( const CTakeDamageInfo &info )
{
	CBaseEntity *pAttacker = info.GetAttacker();
	if ( pAttacker && !pAttacker->IsPlayer() )
	{
		pAttacker = NULL;
	}

	if ( pAttacker == GetBuilder() )
	{
		DetonateObject();
	}

	return BaseClass::OnTakeDamage( info );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CObjectBeartrap::IsPlacementPosValid( void )
{
	bool bResult = BaseClass::IsPlacementPosValid();

	if ( !bResult )
	{
		return false;
	}

	// m_vecBuildOrigin is the proposed build origin

	// start above the trap position
	Vector vecTestPos = m_vecBuildOrigin;
	vecTestPos.z += BEARTRAP_MAXS.z;

	// make sure we can fit a player on top in this pos
	trace_t tr;
	UTIL_TraceHull( vecTestPos, vecTestPos, VEC_HULL_MIN, VEC_HULL_MAX, MASK_PLAYERSOLID, this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr );

	return ( tr.fraction >= 1.0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CObjectBeartrap::DrawDebugTextOverlays( void )
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if ( m_debugOverlays & OVERLAY_TEXT_BIT )
	{
		char tempstr[512];
		Q_snprintf( tempstr, sizeof( tempstr ), "State: %d", m_iState.Get() );
		EntityText( text_offset, tempstr, 0 );
		text_offset++;
	}

	return text_offset;
}
