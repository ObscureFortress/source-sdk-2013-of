//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Worker Node's Fort
//
// $NoKeywords: $
//=============================================================================//

#ifndef FO_OBJ_FORT_H
#define FO_OBJ_FORT_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_obj.h"

class CTFPlayer;

// ------------------------------------------------------------------------ //
// Resupply object that's built by the player
// ------------------------------------------------------------------------ //
class CObjectWorkerFort : public CBaseObject
{
	DECLARE_CLASS(CObjectWorkerFort, CBaseObject);

public:
	DECLARE_SERVERCLASS();

	CObjectWorkerFort();
	~CObjectWorkerFort();

	static CObjectWorkerFort* Create(const Vector &vOrigin, const QAngle &vAngles);

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

#endif // FO_OBJ_WALL_H
