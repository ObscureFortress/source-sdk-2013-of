//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_OBJ_BEARTRAP_H
#define C_OBJ_BEARTRAP_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseobject.h"
#include "ObjectControlPanel.h"

class C_ObjectBeartrap : public C_BaseObject
{
	DECLARE_CLASS( C_ObjectBeartrap, C_BaseObject );
public:
	DECLARE_CLIENTCLASS();

	C_ObjectBeartrap();

	virtual void OnPreDataChanged( DataUpdateType_t updateType );
	virtual void OnDataChanged( DataUpdateType_t updateType );

	virtual void GetStatusText( wchar_t *pStatus, int iMaxStatusLen );
	virtual void GetTargetIDDataString( wchar_t *sDataString, int iMaxLenInBytes );

	virtual void UpdateOnRemove();

	virtual CStudioHdr *OnNewModel( void );

	virtual bool IsPlacementPosValid( void );

	int GetState( void ) { return m_iState; }

	virtual void UpdateDamageEffects( BuildingDamageLevel_t damageLevel );

private:
	int m_iState;
	int m_iOldState;

	CNewParticleEffect *m_pDamageEffects;

private:
	C_ObjectBeartrap( const C_ObjectBeartrap & ); // not defined, not accessible
};

#endif	//C_OBJ_BEARTRAP_H
