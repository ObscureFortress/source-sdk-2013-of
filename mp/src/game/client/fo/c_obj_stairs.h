//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_OBJ_STAIRS_H
#define C_OBJ_STAIRS_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseobject.h"
#include "ObjectControlPanel.h"
#include "vgui_controls/RotatingProgressBar.h"

class C_ObjectWorkerStairs : public C_BaseObject
{
	DECLARE_CLASS( C_ObjectWorkerStairs, C_BaseObject );
public:
	DECLARE_CLIENTCLASS();

	C_ObjectWorkerStairs();
	~C_ObjectWorkerStairs();

	virtual void GetStatusText( wchar_t *pStatus, int iMaxStatusLen );

	virtual void OnGoActive( void );

	virtual void OnDataChanged( DataUpdateType_t updateType );

	virtual void UpdateDamageEffects( BuildingDamageLevel_t damageLevel );


private:

	bool m_bPlayingSound;

	CNewParticleEffect *m_pDamageEffects;

private:
	C_ObjectWorkerStairs( const C_ObjectWorkerStairs & ); // not defined, not accessible
};

#endif	//C_OBJ_WALL_H