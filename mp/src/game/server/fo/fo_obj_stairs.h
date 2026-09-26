//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Worker Node's Stairs
//
// $NoKeywords: $
//=============================================================================//

#ifndef FO_OBJ_STAIRS_H
#define FO_OBJ_STAIRS_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_obj.h"

class CTFPlayer;

// ------------------------------------------------------------------------ //
// Resupply object that's built by the player
// ------------------------------------------------------------------------ //
class CObjectWorkerStairs : public CBaseObject
{
	DECLARE_CLASS(CObjectWorkerStairs, CBaseObject);

public:
	DECLARE_SERVERCLASS();

	CObjectWorkerStairs();
	~CObjectWorkerStairs();

	static CObjectWorkerStairs* Create(const Vector &vOrigin, const QAngle &vAngles);

	virtual void	Spawn();

	virtual void	Precache();
	virtual bool	ClientCommand(CTFPlayer *pPlayer, const CCommand &args);

	virtual void	DetonateObject(void);
	virtual void	OnGoActive(void);
	virtual bool	StartBuilding(CBaseEntity *pBuilder);
	virtual void	SetModel(const char *pModel);

	virtual int	ObjectCaps(void) { return (BaseClass::ObjectCaps() | FCAP_IMPULSE_USE); }

	DECLARE_DATADESC();
};

#endif // FO_OBJ_STAIRS_H
