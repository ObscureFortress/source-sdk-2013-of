//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client's CObjectBeartrap
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_baseobject.h"
#include "c_tf_player.h"
#include "vgui/ILocalize.h"
#include "c_obj_beartrap.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

#define BEARTRAP_MAXS			Vector( 24, 24, 12 )

//-----------------------------------------------------------------------------
// Purpose: Beartrap object
//-----------------------------------------------------------------------------

IMPLEMENT_CLIENTCLASS_DT( C_ObjectBeartrap, DT_ObjectBeartrap, CObjectBeartrap )
	RecvPropInt( RECVINFO( m_iState ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_ObjectBeartrap::C_ObjectBeartrap()
{
	m_iState = 0;
	m_iOldState = 0;
	m_pDamageEffects = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectBeartrap::UpdateOnRemove( void )
{
	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectBeartrap::GetStatusText( wchar_t *pStatus, int iMaxStatusLen )
{
	float flHealthPercent = (float)GetHealth() / (float)GetMaxHealth();
	wchar_t wszHealthPercent[32];
	_snwprintf( wszHealthPercent, sizeof( wszHealthPercent ) / sizeof( wchar_t ) - 1, L"%d%%", (int)( flHealthPercent * 100 ) );

	wchar_t *pszTemplate;

	if ( IsBuilding() )
	{
		pszTemplate = g_pVGuiLocalize->Find( "#FO_ObjStatus_Beartrap_Building" );
	}
	else
	{
		pszTemplate = g_pVGuiLocalize->Find( "#FO_ObjStatus_Beartrap" );
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
//-----------------------------------------------------------------------------
void C_ObjectBeartrap::GetTargetIDDataString( wchar_t *sDataString, int iMaxLenInBytes )
{
	sDataString[0] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectBeartrap::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );

	m_iOldState = m_iState;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectBeartrap::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( m_iOldState != m_iState )
	{
		m_iOldState = m_iState;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CStudioHdr *C_ObjectBeartrap::OnNewModel( void )
{
	return BaseClass::OnNewModel();
}

//-----------------------------------------------------------------------------
// Purpose: Damage level has changed, update our effects
//-----------------------------------------------------------------------------
void C_ObjectBeartrap::UpdateDamageEffects( BuildingDamageLevel_t damageLevel )
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
		pszEffect = "tpdamage_1";
		break;
	case BUILDING_DAMAGE_LEVEL_MEDIUM:
		pszEffect = "tpdamage_2";
		break;
	case BUILDING_DAMAGE_LEVEL_HEAVY:
		pszEffect = "tpdamage_3";
		break;
	case BUILDING_DAMAGE_LEVEL_CRITICAL:
		pszEffect = "tpdamage_4";
		break;

	default:
		break;
	}

	if ( Q_strlen( pszEffect ) > 0 )
	{
		m_pDamageEffects = ParticleProp()->Create( pszEffect, PATTACH_ABSORIGIN );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C_ObjectBeartrap::IsPlacementPosValid( void )
{
	bool bResult = BaseClass::IsPlacementPosValid();

	if ( !bResult )
	{
		return false;
	}

	// m_vecBuildOrigin is the proposed build origin

	// start above the trap position
	Vector vecTestPos = m_vecBuildOrigin;
	vecTestPos.z += BEARTRAP_MAXS.z;

	// make sure we can fit a player on top in this pos
	trace_t tr;
	UTIL_TraceHull( vecTestPos, vecTestPos, VEC_HULL_MIN, VEC_HULL_MAX, MASK_PLAYERSOLID, this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr );

	return ( tr.fraction >= 1.0 );
}
