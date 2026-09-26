//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Worker Node's Fort
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include "fo_obj_fort.h"
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
#define FORT_MODEL_PLACEMENT	"models/buildables/fort_blueprint.mdl"
#define FORT_MODEL_BUILDING		"models/buildables/fort.mdl"
#define FORT_MODEL				"models/buildables/fort_light.mdl"

#define FORT_MINS			Vector( -20, -20, 0)
#define FORT_MAXS			Vector( 20, 20, 55)	// tweak me

IMPLEMENT_SERVERCLASS_ST(CObjectWorkerFort, DT_ObjectWorkerFort)
END_SEND_TABLE()

BEGIN_DATADESC(CObjectWorkerFort)
END_DATADESC()


LINK_ENTITY_TO_CLASS(obj_fort, CObjectWorkerFort);
PRECACHE_REGISTER(obj_fort);

#define FORT_MAX_HEALTH		450

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectWorkerFort::CObjectWorkerFort()
{
	SetMaxHealth(FORT_MAX_HEALTH);
	m_iHealth = FORT_MAX_HEALTH;
	UseClientSideAnimation();

	SetType(OBJ_FORT);
}

CObjectWorkerFort::~CObjectWorkerFort()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerFort::Spawn()
{
	SetModel(FORT_MODEL_PLACEMENT);
	//SetSolid(SOLID_VPHYSICS);
	VPhysicsInitNormal(SOLID_VPHYSICS, 0, true);
	SetCollisionGroup(COLLISION_GROUP_PLAYER_MOVEMENT);

	UTIL_SetSize(this, FORT_MINS, FORT_MAXS);
	m_takedamage = DAMAGE_YES;

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Start building the object
//-----------------------------------------------------------------------------
bool CObjectWorkerFort::StartBuilding(CBaseEntity *pBuilder)
{
	SetModel(FORT_MODEL_BUILDING);

	CreateBuildPoints();

	return BaseClass::StartBuilding(pBuilder);
}

void CObjectWorkerFort::SetModel(const char *pModel)
{
	BaseClass::SetModel(pModel);
	UTIL_SetSize(this, FORT_MINS, FORT_MAXS);
}

//-----------------------------------------------------------------------------
// Purpose: Finished building
//-----------------------------------------------------------------------------
void CObjectWorkerFort::OnGoActive(void)
{
	CTFPlayer *pBuilder = GetBuilder();

	Assert(pBuilder);

	if (!pBuilder)
		return;

	SetModel(FORT_MODEL);

	BaseClass::OnGoActive();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerFort::Precache()
{
	BaseClass::Precache();

	int iModelIndex;

	PrecacheModel(FORT_MODEL_PLACEMENT);

	iModelIndex = PrecacheModel(FORT_MODEL_BUILDING);
	PrecacheGibsForModel(iModelIndex);

	iModelIndex = PrecacheModel(FORT_MODEL);
	PrecacheGibsForModel(iModelIndex);
}

//-----------------------------------------------------------------------------
// If detonated, do some damage
//-----------------------------------------------------------------------------
void CObjectWorkerFort::DetonateObject(void)
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
bool CObjectWorkerFort::ClientCommand(CTFPlayer *pPlayer, const CCommand &args)
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
