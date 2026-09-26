//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client's CObjectFort
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_baseobject.h"
#include "c_tf_player.h"
#include "vgui/ILocalize.h"
#include "c_obj_fort.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Wall object
//-----------------------------------------------------------------------------

IMPLEMENT_CLIENTCLASS_DT(C_ObjectWorkerFort, DT_ObjectWorkerFort, CObjectWorkerFort)
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_ObjectWorkerFort::C_ObjectWorkerFort()
{
	m_bPlayingSound = false;

	m_pDamageEffects = NULL;
}

C_ObjectWorkerFort::~C_ObjectWorkerFort()
{
}

void C_ObjectWorkerFort::GetStatusText( wchar_t *pStatus, int iMaxStatusLen )
{
	float flHealthPercent = (float)GetHealth() / (float)GetMaxHealth();
	wchar_t wszHealthPercent[32];
	_snwprintf(wszHealthPercent, sizeof(wszHealthPercent)/sizeof(wchar_t) - 1, L"%d%%", (int)( flHealthPercent * 100 ) );

	wchar_t *pszTemplate;

	if ( IsBuilding() )
	{
		pszTemplate = g_pVGuiLocalize->Find( "#FO_ObjStatus_Fort_Building" );
	}
	else
	{
		pszTemplate = g_pVGuiLocalize->Find( "#FO_ObjStatus_Fort" );
	}

	if ( pszTemplate )
	{
		g_pVGuiLocalize->ConstructString( pStatus, iMaxStatusLen, pszTemplate,
			1,
			wszHealthPercent );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : updateType - 
//-----------------------------------------------------------------------------
void C_ObjectWorkerFort::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
}

//-----------------------------------------------------------------------------
// Purpose: Damage level has changed, update our effects
//-----------------------------------------------------------------------------
void C_ObjectWorkerFort::UpdateDamageEffects( BuildingDamageLevel_t damageLevel )
{
	if ( m_pDamageEffects )
	{
		m_pDamageEffects->StopEmission( false, false );
		m_pDamageEffects = NULL;
	}

	const char *pszEffect = "";

	switch( damageLevel )
	{
	case BUILDING_DAMAGE_LEVEL_LIGHT:
		pszEffect = "dispenserdamage_1";
		break;
	case BUILDING_DAMAGE_LEVEL_MEDIUM:
		pszEffect = "dispenserdamage_2";
		break;
	case BUILDING_DAMAGE_LEVEL_HEAVY:
		pszEffect = "dispenserdamage_3";
		break;
	case BUILDING_DAMAGE_LEVEL_CRITICAL:
		pszEffect = "dispenserdamage_4";
		break;

	default:
		break;
	}

	if ( Q_strlen(pszEffect) > 0 )
	{
		m_pDamageEffects = ParticleProp()->Create( pszEffect, PATTACH_ABSORIGIN );
	}
}
