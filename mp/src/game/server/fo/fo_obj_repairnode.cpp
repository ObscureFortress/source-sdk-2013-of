//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Worker Nodes's Repair Node
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include "fo_obj_repairnode.h"
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
#define REPAIR_MODEL_PLACEMENT	"models/buildables/repair_level1.mdl"
#define REPAIR_MODEL_BUILDING	"models/buildables/repairnode_heavy.mdl"
#define REPAIR_MODEL				"models/buildables/repairnode_light.mdl"

#define REPAIR_MINS			Vector( -20, -20, 0)
#define REPAIR_MAXS			Vector( 20, 20, 55)	// tweak me

#define REPAIR_TRIGGER_MINS			Vector(-70, -70, 0)
#define REPAIR_TRIGGER_MAXS			Vector( 70,  70, 50)	// tweak me

#define REPAIR_CONTEXT		"RepairContext"

//-----------------------------------------------------------------------------
// Purpose: SendProxy that converts the Healing list UtlVector to entindices
//-----------------------------------------------------------------------------
void SendProxy_RepairList(const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID)
{
	CObjectRepairnode *pRepairnode = (CObjectRepairnode*)pStruct;

	// If this assertion fails, then SendProxyArrayLength_HealingArray must have failed.
	Assert(iElement < pRepairnode->m_hRepairTargets.Size());

	CBaseEntity *pEnt = pRepairnode->m_hRepairTargets[iElement].Get();
	EHANDLE hOther = pEnt;

	SendProxy_EHandleToInt(pProp, pStruct, &hOther, pOut, iElement, objectID);
}

int SendProxyArrayLength_RepairArray(const void *pStruct, int objectID)
{
	CObjectRepairnode *pRepairnode = (CObjectRepairnode*)pStruct;
	return pRepairnode->m_hRepairTargets.Count();
}

IMPLEMENT_SERVERCLASS_ST(CObjectRepairnode, DT_ObjectRepairnode)
SendPropArray2(
	SendProxyArrayLength_RepairArray,
	SendPropInt("repair_array_element", 0, SIZEOF_IGNORE, NUM_NETWORKED_EHANDLE_BITS, SPROP_UNSIGNED, SendProxy_RepairList),
	MAX_PLAYERS,
	0,
	"repair_array"
)

END_SEND_TABLE()

BEGIN_DATADESC(CObjectRepairnode)
DEFINE_THINKFUNC(RepairThink),
END_DATADESC()


LINK_ENTITY_TO_CLASS(obj_repairnode, CObjectRepairnode);
PRECACHE_REGISTER(obj_repairnode);

#define REPAIR_MAX_HEALTH	150

ConVar obj_repairnode_heal_rate("obj_repairnode_heal_rate", "10.0", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY);

class CRepairnodeTouchTrigger : public CBaseTrigger
{
	DECLARE_CLASS(CRepairnodeTouchTrigger, CBaseTrigger);

public:
	CRepairnodeTouchTrigger() {}

	void Spawn(void)
	{
		BaseClass::Spawn();
		//AddSpawnFlags(SF_TRIGGER_ALLOW_ALL);
		InitTrigger();
		SetSolid(SOLID_BBOX);
		UTIL_SetSize(this, Vector(-140, -140, -140), Vector(140, 140, 140));
	}

	virtual void StartTouch(CBaseEntity *pEntity)
	{
		CBaseEntity *pParent = GetOwnerEntity();

		if (pParent)
		{
			pParent->StartTouch(pEntity);
		}
	}

	virtual void EndTouch(CBaseEntity *pEntity)
	{
		CBaseEntity *pParent = GetOwnerEntity();

		if (pParent)
		{
			pParent->EndTouch(pEntity);
		}
	}
};

LINK_ENTITY_TO_CLASS(repairnode_touch_trigger, CRepairnodeTouchTrigger);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectRepairnode::CObjectRepairnode()
{
	SetMaxHealth(REPAIR_MAX_HEALTH);
	m_iHealth = REPAIR_MAX_HEALTH;
	UseClientSideAnimation();

	m_hTouchingEntities.Purge();

	SetType(OBJ_REPAIRNODE);
}

CObjectRepairnode::~CObjectRepairnode()
{
	if (m_hTouchTrigger.Get())
	{
		UTIL_Remove(m_hTouchTrigger);
	}

	int iSize = m_hRepairTargets.Count();
	for (int i = iSize - 1; i >= 0; i--)
	{
		EHANDLE hOther = m_hRepairTargets[i];

		StopHealing(hOther);
	}

	StopSound("Building_Dispenser.Idle");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectRepairnode::Spawn()
{
	SetModel(REPAIR_MODEL_PLACEMENT);
	SetSolid(SOLID_BBOX);

	UTIL_SetSize(this, REPAIR_MINS, REPAIR_MAXS);
	m_takedamage = DAMAGE_YES;

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Start building the object
//-----------------------------------------------------------------------------
bool CObjectRepairnode::StartBuilding(CBaseEntity *pBuilder)
{
	SetModel(REPAIR_MODEL_BUILDING);

	CreateBuildPoints();

	return BaseClass::StartBuilding(pBuilder);
}

void CObjectRepairnode::SetModel(const char *pModel)
{
	BaseClass::SetModel(pModel);
	UTIL_SetSize(this, REPAIR_MINS, REPAIR_MAXS);
}

//-----------------------------------------------------------------------------
// Purpose: Finished building
//-----------------------------------------------------------------------------
void CObjectRepairnode::OnGoActive(void)
{
	CTFPlayer *pBuilder = GetBuilder();

	Assert(pBuilder);

	if (!pBuilder)
		return;

	SetModel(REPAIR_MODEL);

	// Begin thinking
	SetContextThink(&CObjectRepairnode::RepairThink, gpGlobals->curtime + 0.1, REPAIR_CONTEXT);

	m_hTouchTrigger = CBaseEntity::Create("repairnode_touch_trigger", GetAbsOrigin(), vec3_angle, this);

	BaseClass::OnGoActive();

	EmitSound("Building_Dispenser.Idle");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectRepairnode::Precache()
{
	BaseClass::Precache();

	int iModelIndex;

	PrecacheModel(REPAIR_MODEL_PLACEMENT);

	iModelIndex = PrecacheModel("models/buildables/repairnode_heavy.mdl");
	PrecacheGibsForModel(iModelIndex);

	iModelIndex = PrecacheModel("models/buildables/repairnode_light.mdl");
	PrecacheGibsForModel(iModelIndex);

	PrecacheScriptSound("Building_Dispenser.Idle");
	PrecacheScriptSound("Building_Dispenser.GenerateMetal");
	PrecacheScriptSound("Building_Dispenser.Heal");

	PrecacheParticleSystem("dispenser_heal_red");
	PrecacheParticleSystem("dispenser_heal_blue");
	PrecacheParticleSystem("dispenser_heal_green");
	PrecacheParticleSystem("dispenser_heal_yellow");
	PrecacheParticleSystem("dispenser_heal_purple");
	PrecacheParticleSystem("dispenser_heal_pink");
}

//-----------------------------------------------------------------------------
// If detonated, do some damage
//-----------------------------------------------------------------------------
void CObjectRepairnode::DetonateObject(void)
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
bool CObjectRepairnode::ClientCommand(CTFPlayer *pPlayer, const CCommand &args)
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

//-----------------------------------------------------------------------------
// Generate ammo over time
//-----------------------------------------------------------------------------
void CObjectRepairnode::RepairThink(void)
{
	if (IsDisabled())
	{
		// Don't heal or dispense ammo
		SetContextThink(&CObjectRepairnode::RepairThink, gpGlobals->curtime + 0.1, REPAIR_CONTEXT);

		// stop healing everyone
		for (int i = m_hRepairTargets.Count() - 1; i >= 0; i--)
		{
			EHANDLE hEnt = m_hRepairTargets[i];

			CBaseEntity *pOther = hEnt.Get();

			if (pOther)
			{
				//StopHealing(pOther);
			}
		}

		return;
	}

	int iNumNearbyBuildings = 0;
	static float flRadius = 140;
	Vector vecOrigin = GetAbsOrigin() + Vector(0, 0, 32);

	CBaseEntity *pListOfNearbyEntities[32];
	int iNumberOfNearbyEntities = UTIL_EntitiesInSphere(pListOfNearbyEntities, 32, vecOrigin, flRadius, FL_OBJECT);
	for (int i = 0; i < iNumberOfNearbyEntities; i++)
	{
		CBaseObject *pBuilding = ToBaseObject(pListOfNearbyEntities[i]);
		//CTFPlayer *pPlayer = ToTFPlayer(pListOfNearbyEntities[i]);

		if (!pBuilding || !pBuilding->IsAlive())
			continue;

		if (pBuilding == this)
			continue;

		if (!IsHealingTarget(pBuilding) && CouldHealTarget(pBuilding))
		{
			pBuilding->Repair(obj_repairnode_heal_rate.GetFloat());
			StartHealing(pBuilding);
		}

		if (IsHealingTarget(pBuilding) && !CouldHealTarget(pBuilding))
			StopHealing(pBuilding);

		iNumNearbyBuildings++;
	}

	// for each player in touching list
	int iSize = m_hTouchingEntities.Count();
	for (int i = iSize - 1; i >= 0; i--)
	{
		EHANDLE hOther = m_hTouchingEntities[i];

		CBaseEntity *pEnt = hOther.Get();
		bool bHealingTarget = IsHealingTarget(pEnt);
		bool bValidHealTarget = CouldHealTarget(pEnt);

		//Warning("yeah i want to heal\n");

		if (bHealingTarget && !bValidHealTarget)
		{
			// if we can't see them, remove them from healing list
			// does nothing if we are not healing them already
			//StopHealing(pEnt);
		}
		else if (!bHealingTarget && bValidHealTarget)
		{
			// if we can see them, add to healing list	
			// does nothing if we are healing them already
			StartHealing(pEnt);
		}
	}

	SetContextThink(&CObjectRepairnode::RepairThink, gpGlobals->curtime + 0.1, REPAIR_CONTEXT);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectRepairnode::StartTouch(CBaseEntity *pOther)
{
	// add to touching entities
	EHANDLE hOther = pOther;
	m_hTouchingEntities.AddToTail(hOther);

	if (!IsBuilding() && !IsDisabled() && CouldHealTarget(pOther) && !IsHealingTarget(pOther))
	{
		// try to start healing them
		StartHealing(pOther);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectRepairnode::EndTouch(CBaseEntity *pOther)
{
	// remove from touching entities
	EHANDLE hOther = pOther;
	m_hTouchingEntities.FindAndRemove(hOther);

	// remove from healing list
	StopHealing(pOther);
}

//-----------------------------------------------------------------------------
// Purpose: Try to start healing this target
//-----------------------------------------------------------------------------
void CObjectRepairnode::StartHealing(CBaseEntity *pOther)
{
	AddHealingTarget(pOther);

	CBaseObject *pBuilding = ToBaseObject(pOther);
	//CTFPlayer *pPlayer = ToTFPlayer(pOther);

	//CTFPlayer *pBuilder = GetBuilder();

	if (pBuilding)
	{
		pBuilding->m_bHealing = true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Stop healing this target
//-----------------------------------------------------------------------------
void CObjectRepairnode::StopHealing(CBaseEntity *pOther)
{
	bool bFound = false;

	EHANDLE hOther = pOther;
	bFound = m_hRepairTargets.FindAndRemove(hOther);

	if (bFound)
	{
		CBaseObject *pBuilding = ToBaseObject(pOther);

		if (pBuilding)
		{
			pBuilding->m_bHealing = false;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Is this a valid heal target? and not already healing them?
//-----------------------------------------------------------------------------
bool CObjectRepairnode::CouldHealTarget(CBaseEntity *pTarget)
{
	if (!pTarget->FVisible(this, CONTENTS_WINDOW))
		return false;

	if (!pTarget->IsPlayer() && pTarget->IsAlive())
	{
		CBaseObject *pBuilding = ToBaseObject(pTarget);
		if (!pBuilding)
			return false;

		// don't heal enemy buildings
		int iBuildingTeam = pBuilding->GetTeamNumber();
		int iTeam = GetTeamNumber();
		if (iBuildingTeam != iTeam)
			return false;

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CObjectRepairnode::AddHealingTarget(CBaseEntity *pOther)
{
	// add to tail
	EHANDLE hOther = pOther;
	m_hRepairTargets.AddToTail(hOther);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CObjectRepairnode::RemoveHealingTarget(CBaseEntity *pOther)
{
	// remove
	EHANDLE hOther = pOther;
	m_hRepairTargets.FindAndRemove(hOther);
}

//-----------------------------------------------------------------------------
// Purpose: Are we healing this target already
//-----------------------------------------------------------------------------
bool CObjectRepairnode::IsHealingTarget(CBaseEntity *pTarget)
{
	EHANDLE hOther = pTarget;
	return m_hRepairTargets.HasElement(hOther);
}
