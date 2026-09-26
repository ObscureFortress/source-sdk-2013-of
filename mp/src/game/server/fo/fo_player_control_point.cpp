//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "fo_player_control_point.h"
#include "player.h"
#include "teamplay_gamerules.h"
#include "team.h"
#include "team_control_point_master.h"
#include "mp_shareddefs.h"
#include "engine/IEngineSound.h"
#include "soundenvelope.h"

BEGIN_DATADESC(CFOPlayerControlPoint)
	DEFINE_KEYFIELD( m_iszPrintName,			FIELD_STRING,	"point_printname" ),
	DEFINE_KEYFIELD( m_iCPGroup,				FIELD_INTEGER,	"point_group" ),
	DEFINE_KEYFIELD( m_iPointIndex,				FIELD_INTEGER,	"point_index" ),
	DEFINE_KEYFIELD( m_bWarnOnCap,				FIELD_BOOLEAN,	"point_warn_on_cap" ),
	DEFINE_KEYFIELD( m_iszWarnSound,			FIELD_STRING,	"point_warn_sound" ),

	DEFINE_KEYFIELD( m_iszCaptureStartSound,	FIELD_STRING,	"point_capture_start_sound" ),
	DEFINE_KEYFIELD( m_iszCaptureEndSound,		FIELD_STRING,	"point_capture_end_sound" ),
	DEFINE_KEYFIELD( m_iszCaptureInProgress,	FIELD_STRING,	"point_capture_progress_sound" ),
	DEFINE_KEYFIELD( m_iszCaptureInterrupted,	FIELD_STRING,	"point_capture_interrupted_sound" ),

//	DEFINE_FIELD( m_iTeam, FIELD_INTEGER ),
//	DEFINE_FIELD( m_iIndex, FIELD_INTEGER ),
//	DEFINE_FIELD( m_TeamData, CUtlVector < perteamdata_t > ),
//	DEFINE_FIELD( m_bPointVisible, FIELD_INTEGER ),
//	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),
//	DEFINE_FIELD( m_iszName, FIELD_STRING ),
//	DEFINE_FIELD( m_bStartDisabled, FIELD_BOOLEAN ),
//	DEFINE_FIELD( m_flLastContestedAt, FIELD_FLOAT ),
//	DEFINE_FIELD( m_pCaptureInProgressSound, CSoundPatch ),

	DEFINE_INPUTFUNC( FIELD_VOID,		"ShowModel",		InputShowModel ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"HideModel",		InputHideModel ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"RoundActivate",	InputRoundActivate ),

	DEFINE_OUTPUT(	m_OnCap,		"OnCap" ),	// these are fired whenever the point changes modes
	DEFINE_OUTPUT(	m_OnCapReset,		"OnCapReset" ),

	DEFINE_OUTPUT(	m_OnOwnerChanged,	"OnOwnerChanged" ),	// this is fired when a player does the work to change the owner

	DEFINE_OUTPUT(	m_OnOwnerExchanged,	"OnOwnerExchanged" ),	// this is fired when a player takes a point another

	DEFINE_THINKFUNC( AnimThink ),
END_DATADESC();

LINK_ENTITY_TO_CLASS( fo_player_control_point, CFOPlayerControlPoint );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CFOPlayerControlPoint::CFOPlayerControlPoint()
{
	m_TeamData.SetSize( GetNumberOfTeams() );
	m_pCaptureInProgressSound = NULL;

#if defined( TF_DLL ) || defined( TF_MOD )
	UseClientSideAnimation();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::Spawn( void )
{
	// Validate our default team
	if ( m_iDefaultOwner < 0 || m_iDefaultOwner >= GetNumberOfTeams() )
	{
		Warning( "team_control_point '%s' has bad point_default_owner.\n", GetDebugName() );
		m_iDefaultOwner = TEAM_UNASSIGNED;
	}

#if defined( TF_DLL ) || defined( TF_MOD )
	if ( m_iszCaptureStartSound == NULL_STRING )
	{
		m_iszCaptureStartSound = AllocPooledString( "Hologram.Start" );
	}
	if ( m_iszCaptureEndSound == NULL_STRING )
	{
		m_iszCaptureEndSound = AllocPooledString( "Hologram.Stop" );
	}
	if ( m_iszCaptureInProgress == NULL_STRING )
	{
		m_iszCaptureInProgress = AllocPooledString( "Hologram.Move" );
	}
	if ( m_iszCaptureInterrupted == NULL_STRING )
	{
		m_iszCaptureInterrupted = AllocPooledString( "Hologram.Interrupted" );
	}
#endif

	Precache();

	InternalSetOwner( m_iDefaultOwner, false );	//init the owner of this point

	SetActive( !m_bStartDisabled );

	BaseClass::Spawn();

	SetPlaybackRate( 1.0 );
	SetThink( &CTeamControlPoint::AnimThink );
	SetNextThink( gpGlobals->curtime + 0.1f );

	if ( FBitSet( m_spawnflags, SF_CAP_POINT_HIDE_MODEL ) )
	{
		AddEffects( EF_NODRAW );
	}

	if ( FBitSet( m_spawnflags, SF_CAP_POINT_HIDE_SHADOW ) )
	{
		AddEffects( EF_NOSHADOW );
	}

	m_flLastContestedAt = -1;

	m_pCaptureInProgressSound = NULL;


}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CFOPlayerControlPoint::KeyValue( const char *szKeyName, const char *szValue )
{	
	if ( !Q_strncmp( szKeyName, "team_capsound_", 14 ) )
	{
		int iTeam = atoi(szKeyName+14);
		Assert( iTeam >= 0 && iTeam < m_TeamData.Count() );

		m_TeamData[iTeam].iszCapSound = AllocPooledString(szValue);
	}
	else if ( !Q_strncmp( szKeyName, "team_model_", 11 ) )
	{
		int iTeam = atoi(szKeyName+11);
		Assert( iTeam >= 0 && iTeam < m_TeamData.Count() );

		m_TeamData[iTeam].iszModel = AllocPooledString(szValue);
	}
	else if ( !Q_strncmp( szKeyName, "team_timedpoints_", 17 ) )
	{
		int iTeam = atoi(szKeyName+17);
		Assert( iTeam >= 0 && iTeam < m_TeamData.Count() );

		m_TeamData[iTeam].iTimedPoints = atoi(szValue);
	}
	else if ( !Q_strncmp( szKeyName, "team_bodygroup_", 15 ) )
	{
		int iTeam = atoi(szKeyName+15);
		Assert( iTeam >= 0 && iTeam < m_TeamData.Count() );

		m_TeamData[iTeam].iModelBodygroup = atoi(szValue);
	}
	else if ( !Q_strncmp( szKeyName, "team_icon_", 10 ) )
	{
		int iTeam = atoi(szKeyName+10);
		Assert( iTeam >= 0 && iTeam < m_TeamData.Count() );

		m_TeamData[iTeam].iszIcon = AllocPooledString(szValue);
	}
	else if ( !Q_strncmp( szKeyName, "team_overlay_", 13 ) )
	{
		int iTeam = atoi(szKeyName+13);
		Assert( iTeam >= 0 && iTeam < m_TeamData.Count() );

		m_TeamData[iTeam].iszOverlay = AllocPooledString(szValue);
	}
	else if ( !Q_strncmp( szKeyName, "team_previouspoint_", 19 ) )
	{
		int iTeam;
		int iPoint = 0;
		sscanf( szKeyName+19, "%d_%d", &iTeam, &iPoint );
		Assert( iTeam >= 0 && iTeam < m_TeamData.Count() );
		Assert( iPoint >= 0 && iPoint < MAX_PREVIOUS_POINTS );
		m_TeamData[iTeam].iszPreviousPoint[iPoint] = AllocPooledString(szValue);
	}
	else
	{
		return BaseClass::KeyValue( szKeyName, szValue );
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::Precache( void )
{
	for ( int i = 0; i < m_TeamData.Count(); i++ )
	{
		// Skip over spectator
		if ( i == TEAM_SPECTATOR )
			continue;

		if ( m_TeamData[i].iszCapSound != NULL_STRING )
		{
			PrecacheScriptSound( STRING(m_TeamData[i].iszCapSound) );
		}

		if ( m_TeamData[i].iszModel != NULL_STRING )
		{
			PrecacheModel( STRING(m_TeamData[i].iszModel) );
		}

		if ( m_TeamData[i].iszIcon != NULL_STRING )
		{
			PrecacheMaterial( STRING( m_TeamData[i].iszIcon ) );
			m_TeamData[i].iIcon = GetMaterialIndex( STRING( m_TeamData[i].iszIcon ) );
			Assert( m_TeamData[i].iIcon != 0 );
		}

		if ( !m_TeamData[i].iIcon )
		{
			Warning( "Invalid hud icon material for team %d in control point '%s' ( point index %d )\n", i, GetDebugName(), GetPointIndex() );
		}

		if ( m_TeamData[i].iszOverlay != NULL_STRING )
		{
			PrecacheMaterial( STRING( m_TeamData[i].iszOverlay ) );
			m_TeamData[i].iOverlay = GetMaterialIndex( STRING( m_TeamData[i].iszOverlay ) );
			Assert( m_TeamData[i].iOverlay != 0 );

			if ( !m_TeamData[i].iOverlay )
			{
				Warning( "Invalid hud overlay material for team %d in control point '%s' ( point index %d )\n", i, GetDebugName(), GetPointIndex() );
			}
		}
	}

	PrecacheScriptSound( STRING( m_iszCaptureStartSound ) );
	PrecacheScriptSound( STRING( m_iszCaptureEndSound ) );
	PrecacheScriptSound( STRING( m_iszCaptureInProgress ) );
	PrecacheScriptSound( STRING( m_iszCaptureInterrupted ) );

	if ( m_iszWarnSound != NULL_STRING )
	{
		PrecacheScriptSound( STRING( m_iszWarnSound ) );
	}

#if defined( TF_DLL ) || defined( TF_MOD )
	PrecacheScriptSound( "Announcer.ControlPointContested" );
#endif
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CFOPlayerControlPoint::AnimThink( void )
{
	StudioFrameAdvance();
	DispatchAnimEvents(this);
	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: Used by ControlMaster to this point to its default owner
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::InputReset( inputdata_t &input )
{
	m_flLastContestedAt = -1;
	InternalSetOwner( m_iDefaultOwner, false );
	ObjectiveResource()->SetOwningTeam( GetPointIndex(), m_iTeam );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::InputShowModel( inputdata_t &input )
{
	RemoveEffects( EF_NODRAW );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::InputHideModel( inputdata_t &input )
{
	AddEffects( EF_NODRAW );
}

//-----------------------------------------------------------------------------
// Purpose: Sent to every entity at round start; fires the output for the team that owns this point
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::InputRoundActivate( inputdata_t &inputdata )
{
	switch ( m_iTeam - FIRST_GAME_TEAM+1 )
	{
	case 1:
		m_OnRoundStartOwnedByTeam1.FireOutput( this, this );
		break;
	case 2:
		m_OnRoundStartOwnedByTeam2.FireOutput( this, this );
		break;
	case 3:
		m_OnRoundStartOwnedByTeam3.FireOutput( this, this );
		break;
	case 4:
		m_OnRoundStartOwnedByTeam4.FireOutput( this, this );
		break;
	case 5:
		m_OnRoundStartOwnedByTeam5.FireOutput( this, this );
		break;
	case 6:
		m_OnRoundStartOwnedByTeam6.FireOutput( this, this );
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFOPlayerControlPoint::GetCurrentHudIconIndex( void )
{
	return m_TeamData[GetOwner()].iIcon;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFOPlayerControlPoint::GetHudIconIndexForTeam( int iGameTeam )
{
	return m_TeamData[iGameTeam].iIcon;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFOPlayerControlPoint::GetHudOverlayIndexForTeam( int iGameTeam )
{
	return m_TeamData[iGameTeam].iOverlay;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CFOPlayerControlPoint::GetPreviousPointForTeam( int iGameTeam, int iPrevPoint )
{
	Assert( iPrevPoint >= 0 && iPrevPoint < MAX_PREVIOUS_POINTS );

	int iRetVal = -1;
	CBaseEntity *pEntity = gEntList.FindEntityByName( NULL, STRING(m_TeamData[iGameTeam].iszPreviousPoint[iPrevPoint]) );

	if ( pEntity )
	{
		CTeamControlPoint *pPoint = dynamic_cast<CTeamControlPoint*>( pEntity );

		if ( pPoint )
		{
			iRetVal = pPoint->GetPointIndex();
		}
	}

	return iRetVal;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::ForceOwner( int iTeam )
{
	InternalSetOwner( iTeam, false, 0, 0 );
	ObjectiveResource()->SetOwningTeam( GetPointIndex(), m_iTeam );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::SetOwner( int iCapTeam, bool bMakeSound, int iNumCappers, int *pCappingPlayers )
{
	if ( TeamplayGameRules()->PointsMayBeCaptured() )
	{
		InternalSetOwner( iCapTeam, bMakeSound, iNumCappers, pCappingPlayers );
		ObjectiveResource()->SetOwningTeam( GetPointIndex(), m_iTeam );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::CaptureStart( void )
{
	IGameEvent *event = gameeventmanager->CreateEvent( "teamplay_point_startcapture" );
	if ( event )
	{
		event->SetInt( "cp", m_iPointIndex );
		event->SetString( "cpname", STRING(m_iszPrintName) );
		event->SetInt( "team", m_iTeam );

		// pCappingPlayers is a null terminated list of player indices

		char capper[8];

		Q_snprintf( capper, sizeof( capper ), "%d", entindex() );

		event->SetString( "cappers", capper );
		event->SetInt( "priority", 7 );

		gameeventmanager->FireEvent( event );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::CaptureEnd( void )
{
	StopLoopingSounds();
	EmitSound( STRING( m_iszCaptureEndSound ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::CaptureInterrupted( bool bBlocked )
{
	StopLoopingSounds();

	const char *pSoundName = NULL;

	if ( bBlocked == true )
	{
		pSoundName = STRING( m_iszCaptureInterrupted );
	}
	else
	{
		pSoundName = STRING( m_iszCaptureInProgress );
		EmitSound( STRING( m_iszCaptureStartSound ) );
	}

	if ( m_pCaptureInProgressSound == NULL && pSoundName != NULL )
	{
		CPASFilter filter( GetAbsOrigin() );

		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
		m_pCaptureInProgressSound = controller.SoundCreate( filter, entindex(), pSoundName );

		controller.Play( m_pCaptureInProgressSound, 1.0, 100 );
	}

}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::StopLoopingSounds( void )
{
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	if ( m_pCaptureInProgressSound )
	{
		controller.SoundDestroy( m_pCaptureInProgressSound );
		m_pCaptureInProgressSound = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sets the new owner of the point, plays the appropriate sound and shows the right model
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::InternalSetOwner( int iCapTeam, bool bMakeSound, int iNumCappers, int *pCappingPlayers )
{
	Assert( iCapTeam >= 0 && iCapTeam < GetNumberOfTeams() );

	int iOldTeam = m_iTeam;

	m_iTeam = iCapTeam;
	ChangeTeam( iCapTeam );

	if ( bMakeSound )
	{
		CBroadcastRecipientFilter filter;
		EmitSound( filter, entindex(), STRING( m_TeamData[m_iTeam].iszCapSound ) );
	}

	// Update visuals
	SetModel( STRING(m_TeamData[m_iTeam].iszModel) );
	SetBodygroup( 0, m_iTeam );
	m_nSkin = ( m_iTeam == TEAM_UNASSIGNED ) ? 2 : (m_iTeam - 2);
	ResetSequence( LookupSequence("idle") );

	// Determine the pose parameters for each team
	for ( int i = 0; i < m_TeamData.Count(); i++ )
	{
		// Skip spectator
		if ( i == TEAM_SPECTATOR )
			continue;

		if ( GetModelPtr() && GetModelPtr()->SequencesAvailable() )
		{
			m_TeamData[i].iTeamPoseParam = LookupPoseParameter( UTIL_VarArgs( "cappoint_%d_percentage", i ) );
		}
		else
		{
			m_TeamData[i].iTeamPoseParam = -1;
		}
	}
	UpdateCapPercentage();

	if ( m_iTeam == TEAM_UNASSIGNED )
	{
		m_OnCapReset.FireOutput( this, this );
	}
	else
	{
		// Remap team to get first game team = 1
		switch ( m_iTeam - FIRST_GAME_TEAM+1 )
		{
		case 1: 
			m_OnCapTeam1.FireOutput( this, this );
			break;
		case 2: 
			m_OnCapTeam2.FireOutput( this, this );
			break;
		case 3:
			m_OnCapTeam3.FireOutput(this, this);
			break;
		case 4:
			m_OnCapTeam4.FireOutput(this, this);
			break;
		case 5:
			m_OnCapTeam5.FireOutput(this, this);
			break;
		case 6:
			m_OnCapTeam6.FireOutput(this, this);
			break;
		default:
			Assert(0);
			break;
		}

		if (iOldTeam != TEAM_UNASSIGNED)
		{
			switch (m_iTeam - FIRST_GAME_TEAM + 1)
			{
			case 1:
				m_OnExchangedToTeam1.FireOutput(this, this);
				break;
			case 2:
				m_OnExchangedToTeam2.FireOutput(this, this);
				break;
			case 3:
				m_OnExchangedToTeam3.FireOutput(this, this);
				break;
			case 4:
				m_OnExchangedToTeam4.FireOutput(this, this);
				break;
			case 5:
				m_OnExchangedToTeam5.FireOutput(this, this);
				break;
			case 6:
				m_OnExchangedToTeam6.FireOutput(this, this);
				break;
			default:
				Assert(0);
				break;
			}

			switch (iOldTeam - FIRST_GAME_TEAM + 1)
			{
			case 1:
				switch (m_iTeam - FIRST_GAME_TEAM + 1)
				{
				case 2:
					m_OnSwappedBetweenTeam1AndTeam2.FireOutput(this, this);
					m_OnExchangedFromTeam1ToTeam2.FireOutput(this, this);
					m_OnExchangedFromTeam1.FireOutput(this, this);
					break;
				case 3:
					m_OnSwappedBetweenTeam1AndTeam3.FireOutput(this, this);
					m_OnExchangedFromTeam1ToTeam3.FireOutput(this, this);
					m_OnExchangedFromTeam1.FireOutput(this, this);
					break;
				case 4:
					m_OnSwappedBetweenTeam1AndTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam1ToTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam1.FireOutput(this, this);
					break;
				case 5:
					m_OnSwappedBetweenTeam1AndTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam1ToTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam1.FireOutput(this, this);
					break;
				case 6:
					m_OnSwappedBetweenTeam1AndTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam1ToTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam1.FireOutput(this, this);
					break;
				default:
					Assert(0);
					break;
				}
				break;
			case 2:
				switch (m_iTeam - FIRST_GAME_TEAM + 1)
				{
				case 1:
					m_OnSwappedBetweenTeam1AndTeam2.FireOutput(this, this);
					m_OnExchangedFromTeam2ToTeam1.FireOutput(this, this);
					m_OnExchangedFromTeam2.FireOutput(this, this);
					break;
				case 3:
					m_OnSwappedBetweenTeam2AndTeam3.FireOutput(this, this);
					m_OnExchangedFromTeam2ToTeam3.FireOutput(this, this);
					m_OnExchangedFromTeam2.FireOutput(this, this);
					break;
				case 4:
					m_OnSwappedBetweenTeam2AndTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam2ToTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam2.FireOutput(this, this);
					break;
				case 5:
					m_OnSwappedBetweenTeam2AndTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam2ToTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam2.FireOutput(this, this);
					break;
				case 6:
					m_OnSwappedBetweenTeam2AndTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam2ToTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam2.FireOutput(this, this);
					break;
				default:
					Assert(0);
					break;
				}
				break;
			case 3:
				switch (m_iTeam - FIRST_GAME_TEAM + 1)
				{
				case 1:
					m_OnSwappedBetweenTeam1AndTeam3.FireOutput(this, this);
					m_OnExchangedFromTeam3ToTeam1.FireOutput(this, this);
					m_OnExchangedFromTeam3.FireOutput(this, this);
					break;
				case 2:
					m_OnSwappedBetweenTeam2AndTeam3.FireOutput(this, this);
					m_OnExchangedFromTeam3ToTeam2.FireOutput(this, this);
					m_OnExchangedFromTeam3.FireOutput(this, this);
					break;
				case 4:
					m_OnSwappedBetweenTeam3AndTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam3ToTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam3.FireOutput(this, this);
					break;
				case 5:
					m_OnSwappedBetweenTeam3AndTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam3ToTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam3.FireOutput(this, this);
					break;
				case 6:
					m_OnSwappedBetweenTeam3AndTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam3ToTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam3.FireOutput(this, this);
					break;
				default:
					Assert(0);
					break;
				}
				break;
			case 4:
				switch (m_iTeam - FIRST_GAME_TEAM + 1)
				{
				case 1:
					m_OnSwappedBetweenTeam1AndTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam4ToTeam1.FireOutput(this, this);
					m_OnExchangedFromTeam4.FireOutput(this, this);
					break;
				case 2:
					m_OnSwappedBetweenTeam2AndTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam4ToTeam2.FireOutput(this, this);
					m_OnExchangedFromTeam4.FireOutput(this, this);
					break;
				case 3:
					m_OnSwappedBetweenTeam3AndTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam4ToTeam3.FireOutput(this, this);
					m_OnExchangedFromTeam4.FireOutput(this, this);
					break;
				case 5:
					m_OnSwappedBetweenTeam4AndTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam4ToTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam4.FireOutput(this, this);
					break;
				case 6:
					m_OnSwappedBetweenTeam4AndTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam4ToTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam4.FireOutput(this, this);
					break;
				default:
					Assert(0);
					break;
				}
				break;
			case 5:
				switch (m_iTeam - FIRST_GAME_TEAM + 1)
				{
				case 1:
					m_OnSwappedBetweenTeam1AndTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam5ToTeam1.FireOutput(this, this);
					m_OnExchangedFromTeam5.FireOutput(this, this);
					break;
				case 2:
					m_OnSwappedBetweenTeam2AndTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam5ToTeam2.FireOutput(this, this);
					m_OnExchangedFromTeam5.FireOutput(this, this);
					break;
				case 3:
					m_OnSwappedBetweenTeam3AndTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam5ToTeam3.FireOutput(this, this);
					m_OnExchangedFromTeam5.FireOutput(this, this);
					break;
				case 4:
					m_OnSwappedBetweenTeam4AndTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam5ToTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam5.FireOutput(this, this);
					break;
				case 6:
					m_OnSwappedBetweenTeam5AndTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam5ToTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam5.FireOutput(this, this);
					break;
				default:
					Assert(0);
					break;
				}
				break;
			case 6:
				switch (m_iTeam - FIRST_GAME_TEAM + 1)
				{
				case 1:
					m_OnSwappedBetweenTeam1AndTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam6ToTeam1.FireOutput(this, this);
					m_OnExchangedFromTeam6.FireOutput(this, this);
					break;
				case 2:
					m_OnSwappedBetweenTeam2AndTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam6ToTeam2.FireOutput(this, this);
					m_OnExchangedFromTeam6.FireOutput(this, this);
					break;
				case 3:
					m_OnSwappedBetweenTeam3AndTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam6ToTeam3.FireOutput(this, this);
					m_OnExchangedFromTeam6.FireOutput(this, this);
					break;
				case 4:
					m_OnSwappedBetweenTeam4AndTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam6ToTeam4.FireOutput(this, this);
					m_OnExchangedFromTeam6.FireOutput(this, this);
					break;
				case 5:
					m_OnSwappedBetweenTeam5AndTeam6.FireOutput(this, this);
					m_OnExchangedFromTeam6ToTeam5.FireOutput(this, this);
					m_OnExchangedFromTeam6.FireOutput(this, this);
					break;
				default:
					Assert(0);
					break;
				}
				break;
			default:
				Assert(0);
				break;
			}
		}
	}

	// If we're playing a sound, this is a true cap by players.
	if ( bMakeSound )	
	{
		if ( iOldTeam > LAST_SHARED_TEAM && iOldTeam != m_iTeam )
		{
			// Make the members of our old team say something
			for ( int i = 1; i <= gpGlobals->maxClients; i++ )
			{
				CBaseMultiplayerPlayer *pPlayer = ToBaseMultiplayerPlayer( UTIL_PlayerByIndex( i ) );
				if ( !pPlayer )
					continue;
				if ( pPlayer->GetTeamNumber() == iOldTeam )
				{
					pPlayer->SpeakConceptIfAllowed( MP_CONCEPT_LOST_CONTROL_POINT );
				}
			}
		}

		for( int i = 0; i < iNumCappers; i++ )
		{
			int playerIndex = pCappingPlayers[i];

			Assert( playerIndex > 0 && playerIndex <= gpGlobals->maxClients );

			PlayerCapped( ToBaseMultiplayerPlayer(UTIL_PlayerByIndex( playerIndex )) );
		}

		// Remap team to get first game team = 1
		switch ( m_iTeam - FIRST_GAME_TEAM+1 )
		{
		case 1: 
			m_OnOwnerChangedToTeam1.FireOutput( this, this );
			break;
		case 2: 
			m_OnOwnerChangedToTeam2.FireOutput( this, this );
			break;
		case 3:
			m_OnOwnerChangedToTeam3.FireOutput(this, this);
			break;
		}

		if ( m_iTeam != TEAM_UNASSIGNED && iNumCappers )
		{
			SendCapString( m_iTeam, iNumCappers, pCappingPlayers );
		}
	}

	// Have control point master check the win conditions now!
	CBaseEntity *pEnt =	gEntList.FindEntityByClassname( NULL, GetControlPointMasterName() );

	while( pEnt )
	{
		CTeamControlPointMaster *pMaster = dynamic_cast<CTeamControlPointMaster *>( pEnt );

		if ( pMaster->IsActive() )
		{
			pMaster->CheckWinConditions();
		}

		pEnt = gEntList.FindEntityByClassname( pEnt, GetControlPointMasterName() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::SendCapString( int iCapTeam, int iNumCappers, int *pCappingPlayers )
{
	if ( strlen( STRING(m_iszPrintName) ) <= 0 )
		return;

	IGameEvent *event = gameeventmanager->CreateEvent( "teamplay_point_captured" );
	if ( event )
	{
		event->SetInt( "cp", m_iPointIndex );
		event->SetString( "cpname", STRING(m_iszPrintName) );
		event->SetInt( "team", iCapTeam );

		char cappers[9];	// pCappingPlayers is max length 8
		int i;
		for( i=0;i<iNumCappers;i++ )
		{
			cappers[i] = (char)pCappingPlayers[i];
		}

		cappers[i] = '\0';

		// pCappingPlayers is a null terminated list of player indices
		event->SetString( "cappers", cappers );
		event->SetInt( "priority", 9 );

		gameeventmanager->FireEvent( event );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::CaptureBlocked( CBaseMultiplayerPlayer *pPlayer )
{
	if( strlen( STRING(m_iszPrintName) ) <= 0 )
		return;

	IGameEvent *event = gameeventmanager->CreateEvent( "teamplay_capture_blocked" );

	if ( event )
	{
		event->SetInt( "cp", m_iPointIndex );
		event->SetString( "cpname", STRING(m_iszPrintName) );
		event->SetInt( "blocker", pPlayer->entindex() );
		event->SetInt( "priority", 9 );

		gameeventmanager->FireEvent( event );
	}

	PlayerBlocked( pPlayer );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFOPlayerControlPoint::GetOwner( void ) const
{ 
	return m_iPlayer;
}

//-----------------------------------------------------------------------------
// Purpose: Returns the time-based point value of this control point
//-----------------------------------------------------------------------------
int CFOPlayerControlPoint::PointValue( void )
{
	if ( GetOwner() != m_iDefaultOwner )
		return m_TeamData[ GetOwner() ].iTimedPoints;

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::SetActive( bool active )
{
	m_bActive = active;
	
	if( active )
	{
		RemoveEffects( EF_NODRAW );
	}
	else
	{
		AddEffects( EF_NODRAW );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::UpdateCapPercentage( void )
{
	for ( int i = LAST_SHARED_TEAM+1; i < m_TeamData.Count(); i++ )
	{
		// Skip spectator
		if ( i == TEAM_SPECTATOR )
			continue;

		float flPerc = GetTeamCapPercentage(i);

		if ( m_TeamData[i].iTeamPoseParam != -1 )
		{
			SetPoseParameter( m_TeamData[i].iTeamPoseParam, flPerc );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CFOPlayerControlPoint::GetTeamCapPercentage( int iTeam )
{
	int iCappingTeam = ObjectiveResource()->GetCappingTeam( GetPointIndex() );
	if ( iCappingTeam == TEAM_UNASSIGNED )
	{
		// No-one's capping this point.
		if ( iTeam == m_iTeam )
			return 1.0;

		return 0.0;
	}

	float flCapPerc = ObjectiveResource()->GetCPCapPercentage( GetPointIndex() );
	if ( iTeam == iCappingTeam )
		return (1.0 - flCapPerc);
	if ( iTeam == m_iTeam )
		return flCapPerc;

	return 0.0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CFOPlayerControlPoint::LastContestedAt( void )
{
	return m_flLastContestedAt;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::SetLastContestedAt( float flTime )
{
	m_flLastContestedAt = flTime;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFOPlayerControlPoint::DrawDebugTextOverlays( void )
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[1024];
		Q_snprintf(tempstr, sizeof(tempstr), "INDEX: (%d)", GetPointIndex() );
		EntityText(text_offset,tempstr,0);
		text_offset++;

		for ( int i = 0; i < MAX_CONTROL_POINT_TEAMS; i++ )
		{
			if ( ObjectiveResource()->GetBaseControlPointForTeam(i) == GetPointIndex() )
			{
				Q_snprintf(tempstr, sizeof(tempstr), "Base Control Point for Team %d", i );
				EntityText(text_offset,tempstr,0);
				text_offset++;
			}
		}
	}

	return text_offset;
}

//-----------------------------------------------------------------------------
// Purpose: The specified player took part in capping this point.
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::PlayerCapped( CBaseMultiplayerPlayer *pPlayer )
{
	if ( pPlayer )
	{
		pPlayer->SpeakConceptIfAllowed( MP_CONCEPT_CAPTURED_POINT );
	}
}

//-----------------------------------------------------------------------------
// Purpose: The specified player blocked the enemy team from capping this point.
//-----------------------------------------------------------------------------
void CFOPlayerControlPoint::PlayerBlocked( CBaseMultiplayerPlayer *pPlayer )
{
	if ( pPlayer )
	{
		pPlayer->SpeakConceptIfAllowed( MP_CONCEPT_CAPTURE_BLOCKED );
	}
}

