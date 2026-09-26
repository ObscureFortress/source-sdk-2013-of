//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Worker Node's Repair Node
//
// $NoKeywords: $
//=============================================================================//

#ifndef FO_OBJ_REPAIRNODE_H
#define FO_OBJ_REPAIRNODE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_obj.h"

class CTFPlayer;

// ------------------------------------------------------------------------ //
// Resupply object that's built by the player
// ------------------------------------------------------------------------ //
class CObjectRepairnode : public CBaseObject
{
	DECLARE_CLASS(CObjectRepairnode, CBaseObject);

public:
	DECLARE_SERVERCLASS();

	CObjectRepairnode();
	~CObjectRepairnode();

	static CObjectRepairnode* Create(const Vector &vOrigin, const QAngle &vAngles);

	virtual void	Spawn();
	virtual void	Precache();
	virtual bool	ClientCommand(CTFPlayer *pPlayer, const CCommand &args);

	virtual void	DetonateObject(void);
	virtual void	OnGoActive(void);
	virtual bool	StartBuilding(CBaseEntity *pBuilder);
	virtual void	SetModel(const char *pModel);

	void RepairThink(void);

	virtual void StartTouch(CBaseEntity *pOther);
	virtual void EndTouch(CBaseEntity *pOther);

	virtual int	ObjectCaps(void) { return (BaseClass::ObjectCaps() | FCAP_IMPULSE_USE); }


	void StartHealing(CBaseEntity *pOther);
	void StopHealing(CBaseEntity *pOther);

	void AddHealingTarget(CBaseEntity *pOther);
	void RemoveHealingTarget(CBaseEntity *pOther);
	bool IsHealingTarget(CBaseEntity *pTarget);

	bool CouldHealTarget(CBaseEntity *pTarget);

	Vector GetHealOrigin(void);

	CUtlVector< EHANDLE >	m_hRepairTargets;

private:

	//CNetworkArray( EHANDLE, m_hHealingTargets, MAX_DISPENSER_HEALING_TARGETS );


	// Entities currently being touched by this trigger
	CUtlVector< EHANDLE >	m_hTouchingEntities;

	EHANDLE m_hTouchTrigger;
	string_t m_szTriggerName;

	DECLARE_DATADESC();
};

#endif // FO_OBJ_REPAIRNODE_H
