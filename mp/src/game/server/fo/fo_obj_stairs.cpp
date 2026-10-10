//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Worker Node's Stairs
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include "fo_obj_stairs.h"
#include "engine/IEngineSound.h"
#include "tf_player.h"
#include "tf_team.h"
#include "vguiscreen.h"
#include "world.h"
#include "explode.h"
#include "triggers.h"
#include "collisionproperty.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Ground placed version
#define STAIRS_MODEL_PLACEMENT	"models/buildables/stairs_blueprint.mdl"
#define STAIRS_MODEL_BUILDING	"models/buildables/stairs.mdl"
#define STAIRS_MODEL			"models/buildables/stairs_light.mdl"

#define STAIRS_MINS			Vector( -20, -20, 0)
#define STAIRS_MAXS			Vector( 20, 20, 55)	// tweak me

IMPLEMENT_SERVERCLASS_ST(CObjectWorkerStairs, DT_ObjectWorkerStairs)
END_SEND_TABLE()

BEGIN_DATADESC(CObjectWorkerStairs)
END_DATADESC()


LINK_ENTITY_TO_CLASS(obj_stairs, CObjectWorkerStairs);
PRECACHE_REGISTER(obj_stairs);

#define STAIRS_MAX_HEALTH		100

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectWorkerStairs::CObjectWorkerStairs()
{
	SetMaxHealth(STAIRS_MAX_HEALTH);
	m_iHealth = STAIRS_MAX_HEALTH;
	UseClientSideAnimation();

	SetType(OBJ_STAIRS);
}

CObjectWorkerStairs::~CObjectWorkerStairs()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerStairs::Spawn()
{
	SetModel(STAIRS_MODEL_PLACEMENT);
	SetSolid(SOLID_BBOX);

	UTIL_SetSize(this, STAIRS_MINS, STAIRS_MAXS);
	m_takedamage = DAMAGE_YES;

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Start building the object
//-----------------------------------------------------------------------------
bool CObjectWorkerStairs::StartBuilding(CBaseEntity *pBuilder)
{
	SetModel(STAIRS_MODEL_BUILDING);

	CreateBuildPoints();

	return BaseClass::StartBuilding(pBuilder);
}

void CObjectWorkerStairs::SetModel(const char *pModel)
{
	BaseClass::SetModel(pModel);
	UTIL_SetSize(this, STAIRS_MINS, STAIRS_MAXS);
}

//-----------------------------------------------------------------------------
// Purpose: Finished building
//-----------------------------------------------------------------------------
void CObjectWorkerStairs::OnGoActive(void)
{
	CTFPlayer *pBuilder = GetBuilder();

	Assert(pBuilder);

	if (!pBuilder)
		return;

	SetModel(STAIRS_MODEL);

	// Players can walk on the finished stairs
	SetSolid(SOLID_VPHYSICS);
	VPhysicsInitStatic();
	CollisionProp()->SetSurroundingBoundsType(USE_HITBOXES);

	BaseClass::OnGoActive();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerStairs::Precache()
{
	BaseClass::Precache();

	int iModelIndex;

	PrecacheModel(STAIRS_MODEL_PLACEMENT);

	iModelIndex = PrecacheModel(STAIRS_MODEL_BUILDING);
	PrecacheGibsForModel(iModelIndex);

	iModelIndex = PrecacheModel(STAIRS_MODEL);
	PrecacheGibsForModel(iModelIndex);
}

//-----------------------------------------------------------------------------
// If detonated, do some damage
//-----------------------------------------------------------------------------
void CObjectWorkerStairs::DetonateObject(void)
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
bool CObjectWorkerStairs::ClientCommand(CTFPlayer *pPlayer, const CCommand &args)
{
	const char *pCmd = args[0];
	if (FStrEq(pCmd, "use"))
	{
		// I can't do anything if I'm not active
		if (!ShouldBeActive())
			return true;

		return true;
	}
	else if (FStrEq(pCmd, "repair"))
	{
		Command_Repair(pPlayer);
		return true;
	}

	return BaseClass::ClientCommand(pPlayer, args);
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
