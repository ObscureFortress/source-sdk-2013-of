//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client's CObjectWall
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_baseobject.h"
#include "c_tf_player.h"
#include "vgui/ILocalize.h"
#include "c_obj_wall.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

IMPLEMENT_CLIENTCLASS_DT( C_ObjectWorkerWall, DT_ObjectWorkerWall, CObjectWorkerWall )
	RecvPropInt( RECVINFO( m_iUpgradeLevel ) ),
	RecvPropInt( RECVINFO( m_iState ) ),
	RecvPropInt( RECVINFO( m_iUpgradeMetal ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_ObjectWorkerWall::C_ObjectWorkerWall()
{
	m_pDamageEffects = NULL;
	m_iOldUpgradeLevel = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectWorkerWall::OnGoActive( void )
{
	SetSolid( SOLID_VPHYSICS );
	VPhysicsInitStatic();
	CollisionProp()->SetSurroundingBoundsType( USE_HITBOXES );

	BaseClass::OnGoActive();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : updateType - 
//-----------------------------------------------------------------------------
void C_ObjectWorkerWall::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );

	m_iOldBodygroups = GetBody();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : updateType - 
//-----------------------------------------------------------------------------
void C_ObjectWorkerWall::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( m_iOldUpgradeLevel != m_iUpgradeLevel )
	{
		m_iOldUpgradeLevel = m_iUpgradeLevel;
	}

	// intercept bodygroup sets from the server
	// we aren't clientsideanimating, but we don't want the server setting our
	// bodygroup while we are placing
	if ( IsPlacing() && m_iOldBodygroups != GetBody() )
	{
		m_nBody = m_iOldBodygroups;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectWorkerWall::GetStatusText( wchar_t *pStatus, int iMaxStatusLen )
{
	float flHealthPercent = (float)GetHealth() / (float)GetMaxHealth();
	wchar_t wszHealthPercent[32];
	_snwprintf(wszHealthPercent, sizeof(wszHealthPercent)/sizeof(wchar_t) - 1, L"%d%%", (int)( flHealthPercent * 100 ) );

	if ( IsBuilding() )
	{
		// "Wall Building... 85%" 

		wchar_t *pszTemplate = g_pVGuiLocalize->Find( "#FO_ObjStatus_Wall_Building" );

		if ( pszTemplate )
		{
			g_pVGuiLocalize->ConstructString( pStatus, iMaxStatusLen, pszTemplate,
				1,
				wszHealthPercent );
		}
	}
	else if ( m_iUpgradeLevel == 1 )
	{
		// "Wall ( Level 1 )  Health 100%" 

		wchar_t wszLevel[16]; 

		_snwprintf(wszLevel, sizeof(wszLevel)/sizeof(wchar_t) - 1, L"%d", m_iUpgradeLevel );

		wchar_t *pszTemplate = g_pVGuiLocalize->Find( "#FO_ObjStatus_Wall_Level1" );

		if ( pszTemplate )
		{
			g_pVGuiLocalize->ConstructString( pStatus, iMaxStatusLen, pszTemplate,
				2,
				wszLevel,
				wszHealthPercent );
		}
	}
	else
	{
		// "Wall ( Level 2 )  Health 100%" 

		wchar_t *pszTemplate = g_pVGuiLocalize->Find( "#FO_ObjStatus_Wall_Level2" );

		if ( pszTemplate )
		{
			g_pVGuiLocalize->ConstructString( pStatus, iMaxStatusLen, pszTemplate,
				1,
				wszHealthPercent );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectWorkerWall::DisplayHintTo( C_BasePlayer *pPlayer )
{
	bool bHintPlayed = false;

	C_TFPlayer *pTFPlayer = ToTFPlayer(pPlayer);
	if ( InSameTeam( pPlayer ) )
	{
		// We're looking at a friendly object. 
		if ( pTFPlayer->IsPlayerClass( TF_CLASS_ENGINEER ) || pTFPlayer->IsPlayerClass( FO_CLASS_SENTRONIC + 1 ) || pTFPlayer->IsPlayerClass( FO_CLASS_DISMATIC + 1 ) || pTFPlayer->IsPlayerClass( FO_CLASS_TELECON + 1 ) || pTFPlayer->IsPlayerClass( FO_CLASS_WORKERNODE + 1 ) || pTFPlayer->IsPlayerClass( FO_CLASS_SAPTRAP + 1 ) || pTFPlayer->IsPlayerClass( FO_CLASS_CUSTOM1 + 1 ) || pTFPlayer->IsPlayerClass( FO_CLASS_CUSTOM2 + 1 ) || pTFPlayer->IsPlayerClass( FO_CLASS_CUSTOM3 + 1 ) )
		{
			// If it can be upgraded, tell me whether I still need metal or can upgrade it now
			if ( GetHealth() == GetMaxHealth() && GetUpgradeLevel() == 1 )
			{
				if ( pTFPlayer->GetBuildResources() < SENTRYGUN_UPGRADE_COST )
				{
					bHintPlayed = pTFPlayer->HintMessage( HINT_ENGINEER_METAL_TO_UPGRADE, false, true );
				}
				else
				{
					bHintPlayed = pTFPlayer->HintMessage( HINT_WORKERNODE_UPGRADE_WALL, false, true );
				}
			}
		}
	}

	if ( !bHintPlayed )
	{
		BaseClass::DisplayHintTo( pPlayer );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C_ObjectWorkerWall::IsUpgrading( void ) const
{
	return ( m_iState == WALL_STATE_UPGRADING );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectWorkerWall::GetTargetIDString( wchar_t *sIDString, int iMaxLenInBytes )
{
	BaseClass::GetTargetIDString( sIDString, iMaxLenInBytes );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ObjectWorkerWall::GetTargetIDDataString( wchar_t *sDataString, int iMaxLenInBytes )
{
	sDataString[0] = '\0';

	if ( m_iUpgradeLevel >= 2 )
		return;

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalPlayer )
		return;

	wchar_t wszBuilderName[ MAX_PLAYER_NAME_LENGTH ];
	wchar_t wszObjectName[ 32 ];
	wchar_t wszUpgradeProgress[ 32 ];

	g_pVGuiLocalize->ConvertANSIToUnicode( GetStatusName(), wszObjectName, sizeof(wszObjectName) );

	C_BasePlayer *pBuilder = GetOwner();

	if ( pBuilder )
	{
		g_pVGuiLocalize->ConvertANSIToUnicode( pBuilder->GetPlayerName(), wszBuilderName, sizeof(wszBuilderName) );
	}
	else
	{
		wszBuilderName[0] = '\0';
	}

	// level 1 shows upgrade progress
	_snwprintf( wszUpgradeProgress, ARRAYSIZE(wszUpgradeProgress) - 1, L"%d / %d", m_iUpgradeMetal, WALL_UPGRADE_METAL );
	wszUpgradeProgress[ ARRAYSIZE(wszUpgradeProgress)-1 ] = '\0';

	const char *printFormatString = "#TF_playerid_object_upgrading";

	g_pVGuiLocalize->ConstructString( sDataString, iMaxLenInBytes, g_pVGuiLocalize->Find(printFormatString),
		1,
		wszUpgradeProgress );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *C_ObjectWorkerWall::GetHudStatusIcon( void )
{
	const char *pszResult;

	switch( m_iUpgradeLevel )
	{
	case 1:
	default:
		pszResult = "obj_status_wall_1";
		break;
	case 2:
		pszResult = "obj_status_wall_2";
		break;
	}

	return pszResult;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CStudioHdr *C_ObjectWorkerWall::OnNewModel( void )
{
	CStudioHdr *hdr = BaseClass::OnNewModel();

	UpdateDamageEffects( m_damageLevel );

	// Reset Bodygroups
	for ( int i = GetNumBodyGroups()-1; i >= 0; i-- )
	{
		SetBodygroup( i, 0 );
	}

	SetSolid( SOLID_VPHYSICS );
	VPhysicsInitStatic();
	CollisionProp()->SetSurroundingBoundsType( USE_HITBOXES );

	return hdr;
}

//-----------------------------------------------------------------------------
// Purpose: Damage level has changed, update our effects
//-----------------------------------------------------------------------------
void C_ObjectWorkerWall::UpdateDamageEffects( BuildingDamageLevel_t damageLevel )
{
	if ( m_pDamageEffects )
	{
		ParticleProp()->StopEmission( m_pDamageEffects, false, false );
		m_pDamageEffects = NULL;
	}
}
