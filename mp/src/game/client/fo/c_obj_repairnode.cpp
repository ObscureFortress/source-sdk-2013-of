//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client's CObjectRepairnode
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_baseobject.h"
#include "c_tf_player.h"
#include "vgui/ILocalize.h"
#include "c_obj_repairnode.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: RecvProxy that converts the Team's player UtlVector to entindexes
//-----------------------------------------------------------------------------
void RecvProxy_RepairingList(  const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_ObjectRepairnode *pRepairnode = (C_ObjectRepairnode*)pStruct;

	CBaseHandle *pHandle = (CBaseHandle*)(&(pRepairnode->m_hRepairTargets[pData->m_iElement])); 
	RecvProxy_IntToEHandle( pData, pStruct, pHandle );

	// update the heal beams
	pRepairnode->m_bUpdateRepairTargets = true;
}

void RecvProxyArrayLength_RepairingArray( void *pStruct, int objectID, int currentArrayLength )
{
	C_ObjectRepairnode *pRepairnode = (C_ObjectRepairnode*)pStruct;

	if ( pRepairnode->m_hRepairTargets.Size() != currentArrayLength )
		pRepairnode->m_hRepairTargets.SetSize( currentArrayLength );

	// update the heal beams
	pRepairnode->m_bUpdateRepairTargets = true;
}

//-----------------------------------------------------------------------------
// Purpose: Repair Node object
//-----------------------------------------------------------------------------

IMPLEMENT_CLIENTCLASS_DT(C_ObjectRepairnode, DT_ObjectRepairnode, CObjectRepairnode)
	RecvPropArray2( 
		RecvProxyArrayLength_RepairingArray,
		RecvPropInt( "repair_array_element", 0, SIZEOF_IGNORE, 0, RecvProxy_RepairingList ), 
		MAX_PLAYERS, 
		0, 
		"repair_array"
		)
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_ObjectRepairnode::C_ObjectRepairnode()
{
	m_bUpdateRepairTargets = false;
	m_bPlayingSound = false;

	m_pDamageEffects = NULL;
}

C_ObjectRepairnode::~C_ObjectRepairnode()
{
	StopSound( "Building_Dispenser.Heal" );
}

void C_ObjectRepairnode::GetStatusText( wchar_t *pStatus, int iMaxStatusLen )
{
	float flHealthPercent = (float)GetHealth() / (float)GetMaxHealth();
	wchar_t wszHealthPercent[32];
	_snwprintf(wszHealthPercent, sizeof(wszHealthPercent)/sizeof(wchar_t) - 1, L"%d%%", (int)( flHealthPercent * 100 ) );

	wchar_t *pszTemplate;

	if ( IsBuilding() )
	{
		pszTemplate = g_pVGuiLocalize->Find( "#FO_ObjStatus_Repairnode_Building" );
	}
	else
	{
		pszTemplate = g_pVGuiLocalize->Find( "#FO_ObjStatus_Repairnode" );
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
void C_ObjectRepairnode::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( m_bUpdateRepairTargets )
	{
		UpdateEffects();
		m_bUpdateRepairTargets = false;
	}
}

void C_ObjectRepairnode::UpdateEffects( void )
{
	// Find all the targets we've stopped healing
	bool bStillHealing[MAX_DISPENSER_HEALING_TARGETS];
	for ( int i = 0; i < m_hRepairTargetEffects.Count(); i++ )
	{
		bStillHealing[i] = false;

		// Are we still healing this target?
		for ( int target = 0; target < m_hRepairTargets.Count(); target++ )
		{
			if ( m_hRepairTargets[target] && m_hRepairTargets[target] == m_hRepairTargetEffects[i].pTarget )
			{
				bStillHealing[i] = true;
				break;
			}
		}
	}

	// Now remove all the dead effects
	for ( int i = m_hRepairTargetEffects.Count()-1; i >= 0; i-- )
	{
		if ( !bStillHealing[i] )
		{
			ParticleProp()->StopEmission( m_hRepairTargetEffects[i].pEffect );
			m_hRepairTargetEffects.Remove(i);
		}
	}

	// Now add any new targets
	for ( int i = 0; i < m_hRepairTargets.Count(); i++ )
	{
		C_BaseEntity *pTarget = m_hRepairTargets[i].Get();

		// Loops through the healing targets, and make sure we have an effect for each of them
		if ( pTarget )
		{
			bool bHaveEffect = false;
			for ( int targets = 0; targets < m_hRepairTargetEffects.Count(); targets++ )
			{
				if ( m_hRepairTargetEffects[targets].pTarget == pTarget )
				{
					bHaveEffect = true;
					break;
				}
			}

			if ( bHaveEffect )
				continue;

			const char *pszEffectName;
			if ( GetTeamNumber() == TF_TEAM_RED )
			{
				pszEffectName = "dispenser_heal_red";
			}
			else
			{
				pszEffectName = "dispenser_heal_blue";
			}

			CNewParticleEffect *pEffect = ParticleProp()->Create( pszEffectName, PATTACH_POINT_FOLLOW, "heal_origin" );
			ParticleProp()->AddControlPoint( pEffect, 1, pTarget, PATTACH_ABSORIGIN_FOLLOW, NULL, Vector(0,0,50) );

			int iIndex = m_hRepairTargetEffects.AddToTail();
			m_hRepairTargetEffects[iIndex].pTarget = pTarget;
			m_hRepairTargetEffects[iIndex].pEffect = pEffect;

			// Start the sound over again every time we start a new beam
			StopSound( "Building_Dispenser.Heal" );

			CLocalPlayerFilter filter;
			EmitSound( filter, entindex(), "Building_Dispenser.Heal" );

			m_bPlayingSound = true;
		}
	}

	// Stop the sound if we're not healing anyone
	if ( m_bPlayingSound && m_hRepairTargets.Count() == 0 )
	{
		m_bPlayingSound = false;

		// stop the sound
		StopSound( "Building_Dispenser.Heal" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Damage level has changed, update our effects
//-----------------------------------------------------------------------------
void C_ObjectRepairnode::UpdateDamageEffects( BuildingDamageLevel_t damageLevel )
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