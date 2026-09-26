//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Worker Node's Wall
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include "fo_obj_wall.h"
#include "engine/IEngineSound.h"
#include "tf_player.h"
#include "tf_team.h"
#include "vguiscreen.h"
#include "world.h"
#include "explode.h"
#include "triggers.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Ground placed version
#define WALL_MODEL_PLACEMENT	"models/buildables/wall_blueprint.mdl"
#define WALL_MODEL_BUILDING	"models/buildables/wall.mdl"
#define WALL_MODEL				"models/buildables/wall_light.mdl"

#define WALL_MINS			Vector( -20, -20, 0)
#define WALL_MAXS			Vector( 20, 20, 55)	// tweak me

IMPLEMENT_SERVERCLASS_ST( CObjectWorkerWall, DT_ObjectWorkerWall )
END_SEND_TABLE()

BEGIN_DATADESC( CObjectWorkerWall )
END_DATADESC()


LINK_ENTITY_TO_CLASS(obj_wall, CObjectWorkerWall);
PRECACHE_REGISTER(obj_wall);

#define WALL_MAX_HEALTH		200

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectWorkerWall::CObjectWorkerWall()
{
	SetMaxHealth( WALL_MAX_HEALTH );
	m_iHealth = WALL_MAX_HEALTH;
	UseClientSideAnimation();

	SetType( OBJ_WALL );
}

CObjectWorkerWall::~CObjectWorkerWall()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerWall::Spawn()
{
	SetModel( WALL_MODEL_PLACEMENT );
	//SetSolid( SOLID_VPHYSICS );
	VPhysicsInitNormal(SOLID_VPHYSICS, 0, true);
	SetCollisionGroup(COLLISION_GROUP_PLAYER_MOVEMENT);
	//VPhysicsInitNormal(SOLID_VPHYSICS, 0, false);

	UTIL_SetSize(this, WALL_MINS, WALL_MAXS);
	m_takedamage = DAMAGE_YES;

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Start building the object
//-----------------------------------------------------------------------------
bool CObjectWorkerWall::StartBuilding( CBaseEntity *pBuilder )
{
	SetModel( WALL_MODEL_BUILDING );

	CreateBuildPoints();

	return BaseClass::StartBuilding( pBuilder );
}

void CObjectWorkerWall::SetModel( const char *pModel )
{
	BaseClass::SetModel( pModel );
	UTIL_SetSize(this, WALL_MINS, WALL_MAXS);
}

//-----------------------------------------------------------------------------
// Purpose: Finished building
//-----------------------------------------------------------------------------
void CObjectWorkerWall::OnGoActive( void )
{
	CTFPlayer *pBuilder = GetBuilder();

	Assert( pBuilder );

	if ( !pBuilder )
		return;

	SetModel( WALL_MODEL );

	BaseClass::OnGoActive();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerWall::Precache()
{
	BaseClass::Precache();

	int iModelIndex;

	PrecacheModel( WALL_MODEL_PLACEMENT );

	iModelIndex = PrecacheModel( WALL_MODEL_BUILDING );
	PrecacheGibsForModel( iModelIndex );

	iModelIndex = PrecacheModel( WALL_MODEL );
	PrecacheGibsForModel( iModelIndex );
}

//-----------------------------------------------------------------------------
// If detonated, do some damage
//-----------------------------------------------------------------------------
void CObjectWorkerWall::DetonateObject( void )
{
	/*
	float flDamage = min( 100 + m_iAmmoMetal, 250 );

	ExplosionCreate( 
		GetAbsOrigin(),
		GetAbsAngles(),
		GetBuilder(),
		flDamage,	//magnitude
		flDamage,		//radius
		0,
		0.0f,				//explosion force
		this,				//inflictor
		DMG_BLAST | DMG_HALF_FALLOFF);
	*/

	BaseClass::DetonateObject();
}

//-----------------------------------------------------------------------------
// Handle commands sent from vgui panels on the client 
//-----------------------------------------------------------------------------
bool CObjectWorkerWall::ClientCommand( CTFPlayer *pPlayer, const CCommand &args )
{
	const char *pCmd = args[0];
	if ( FStrEq( pCmd, "use" ) )
	{
		// I can't do anything if I'm not active
		if ( !ShouldBeActive() ) 
			return true;

		return true;
	}
	else if ( FStrEq( pCmd, "repair" ) )
	{
		Command_Repair( pPlayer );
		return true;
	}

	return BaseClass::ClientCommand( pPlayer, args );
}

/*
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CObjectWorkerWall::DrawDebugTextOverlays(void) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];
		Q_snprintf( tempstr, sizeof( tempstr ),"Metal: %d", m_iAmmoMetal.Get() );
		EntityText(text_offset,tempstr,0);
		text_offset++;
	}
	return text_offset;
}
*/
