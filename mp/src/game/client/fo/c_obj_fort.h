//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_OBJ_FORT_H
#define C_OBJ_FORT_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseobject.h"
#include "ObjectControlPanel.h"
#include "vgui_controls/RotatingProgressBar.h"

#define FORT_UPGRADE_METAL	100

class C_ObjectWorkerFort : public C_BaseObject
{
	DECLARE_CLASS( C_ObjectWorkerFort, C_BaseObject );
public:
	DECLARE_CLIENTCLASS();

	C_ObjectWorkerFort();

	int GetUpgradeLevel( void ) { return m_iUpgradeLevel; }
	int GetUpgradeMetal( void ) { return m_iUpgradeMetal; }
	int GetUpgradeMetalRequired( void ) { return FORT_UPGRADE_METAL; }

	virtual void GetStatusText( wchar_t *pStatus, int iMaxStatusLen );

	virtual void OnGoActive( void );

	virtual bool IsUpgrading( void ) const;

	virtual void GetTargetIDString( wchar_t *sIDString, int iMaxLenInBytes );
	virtual void GetTargetIDDataString( wchar_t *sDataString, int iMaxLenInBytes );

	virtual const char *GetHudStatusIcon( void );

	virtual CStudioHdr *OnNewModel( void );

	virtual void UpdateDamageEffects( BuildingDamageLevel_t damageLevel );

	virtual void OnPreDataChanged( DataUpdateType_t updateType );
	virtual void OnDataChanged( DataUpdateType_t updateType );

	virtual void DisplayHintTo( C_BasePlayer *pPlayer );

private:
	int m_iState;
	int m_iUpgradeLevel;
	int m_iOldUpgradeLevel;
	int m_iUpgradeMetal;

	CNewParticleEffect *m_pDamageEffects;

	int m_iPlacementBodygroup;
	int m_iOldBodygroups;

private:
	C_ObjectWorkerFort( const C_ObjectWorkerFort & ); // not defined, not accessible
};

#endif	//C_OBJ_FORT_H
