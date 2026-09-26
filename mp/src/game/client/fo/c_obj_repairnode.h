//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_OBJ_REPAIRNODE_H
#define C_OBJ_REPAIRNODE_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseobject.h"
#include "ObjectControlPanel.h"
#include "vgui_controls/RotatingProgressBar.h"

class C_ObjectRepairnode : public C_BaseObject
{
	DECLARE_CLASS( C_ObjectRepairnode, C_BaseObject );
public:
	DECLARE_CLIENTCLASS();

	C_ObjectRepairnode();
	~C_ObjectRepairnode();

	virtual void GetStatusText( wchar_t *pStatus, int iMaxStatusLen );

	CUtlVector< CHandle<C_TFPlayer> > m_hRepairTargets;

	virtual void OnDataChanged( DataUpdateType_t updateType );

	void UpdateEffects( void );

	virtual void UpdateDamageEffects( BuildingDamageLevel_t damageLevel );

	bool m_bUpdateRepairTargets;

private:

	bool m_bPlayingSound;

	struct repairtargeteffects_t
	{
		C_BaseEntity		*pTarget;
		CNewParticleEffect	*pEffect;
	};
	CUtlVector<repairtargeteffects_t> m_hRepairTargetEffects;

	CNewParticleEffect *m_pDamageEffects;

private:
	C_ObjectRepairnode( const C_ObjectRepairnode & ); // not defined, not accessible
};

#endif	//C_OBJ_REPAIRNODE_H