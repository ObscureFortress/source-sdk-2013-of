//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_OBJ_WALL_H
#define C_OBJ_WALL_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseobject.h"
#include "ObjectControlPanel.h"
#include "vgui_controls/RotatingProgressBar.h"

class C_ObjectWorkerWall : public C_BaseObject
{
	DECLARE_CLASS( C_ObjectWorkerWall, C_BaseObject );
public:
	DECLARE_CLIENTCLASS();

	C_ObjectWorkerWall();
	~C_ObjectWorkerWall();

	virtual void GetStatusText( wchar_t *pStatus, int iMaxStatusLen );

	virtual void OnDataChanged( DataUpdateType_t updateType );

	virtual void UpdateDamageEffects( BuildingDamageLevel_t damageLevel );


private:

	bool m_bPlayingSound;

	CNewParticleEffect *m_pDamageEffects;

private:
	C_ObjectWorkerWall( const C_ObjectWorkerWall & ); // not defined, not accessible
};

#endif	//C_OBJ_WALL_H