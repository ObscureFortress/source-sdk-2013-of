//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/ISurface.h>
#include <vgui/IImage.h>
#include <vgui_controls/Label.h>

#include "c_playerresource.h"
#include "teamplay_round_timer.h"
#include "utlvector.h"
#include "entity_capture_flag.h"
#include "c_tf_player.h"
#include "c_team.h"
#include "c_tf_team.h"
#include "c_team_objectiveresource.h"
#include "tf_hud_flagstatus.h"
#include "fo_hud_generatorstatus.h"
#include "tf_hud_objectivestatus.h"
#include "tf_spectatorgui.h"
#include "teamplayroundbased_gamerules.h"
#include "tf_gamerules.h"

using namespace vgui;

DECLARE_BUILD_FACTORY( CTFProgressBar );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFProgressBar::CTFProgressBar( vgui::Panel *parent, const char *name ) : vgui::ImagePanel( parent, name )
{
	m_flPercent = 0.0f;

	m_iTexture = vgui::surface()->DrawGetTextureId( "hud/objectives_timepanel_progressbar" );
	if ( m_iTexture == -1 ) // we didn't find it, so create a new one
	{
		m_iTexture = vgui::surface()->CreateNewTextureID();	
	}

	vgui::surface()->DrawSetTextureFile( m_iTexture, "hud/objectives_timepanel_progressbar", true, false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProgressBar::Paint()
{
	int wide, tall;
	GetSize( wide, tall );

	float uv1 = 0.0f, uv2 = 1.0f;
	Vector2D uv11( uv1, uv1 );
	Vector2D uv21( uv2, uv1 );
	Vector2D uv22( uv2, uv2 );
	Vector2D uv12( uv1, uv2 );

	vgui::Vertex_t verts[4];	
	verts[0].Init( Vector2D( 0, 0 ), uv11 );
	verts[1].Init( Vector2D( wide, 0 ), uv21 );
	verts[2].Init( Vector2D( wide, tall ), uv22 );
	verts[3].Init( Vector2D( 0, tall ), uv12  );

	// first, just draw the whole thing inactive.
	vgui::surface()->DrawSetTexture( m_iTexture );
	vgui::surface()->DrawSetColor( m_clrInActive );
	vgui::surface()->DrawTexturedPolygon( 4, verts );

	// now, let's calculate the "active" part of the progress bar
	if ( m_flPercent < m_flPercentWarning )
	{
		vgui::surface()->DrawSetColor( m_clrActive );
	}
	else
	{
		vgui::surface()->DrawSetColor( m_clrWarning );
	}

	// we're going to do this using quadrants
	//  -------------------------
	//  |           |           |
	//  |           |           |
	//  |     4     |     1     |
	//  |           |           |
	//  |           |           |
	//  -------------------------
	//  |           |           |
	//  |           |           |
	//  |     3     |     2     |
	//  |           |           |
	//  |           |           |
	//  -------------------------

	float flCompleteCircle = ( 2.0f * M_PI );
	float fl90degrees = flCompleteCircle / 4.0f;

	float flEndAngle = flCompleteCircle * ( 1.0f - m_flPercent ); // count DOWN (counter-clockwise)
	//float flEndAngle = flCompleteCircle * m_flPercent; // count UP (clockwise)

	float flHalfWide = (float)wide / 2.0f;
	float flHalfTall = (float)tall / 2.0f;

	if ( flEndAngle >= fl90degrees * 3.0f ) // >= 270 degrees
	{
		// draw the first and second quadrants
		uv11.Init( 0.5f, 0.0f );
		uv21.Init( 1.0f, 0.0f );
		uv22.Init( 1.0f, 1.0f );
		uv12.Init( 0.5, 1.0f );

		verts[0].Init( Vector2D( flHalfWide, 0.0f ), uv11 );
		verts[1].Init( Vector2D( wide, 0.0f ), uv21 );
		verts[2].Init( Vector2D( wide, tall ), uv22 );
		verts[3].Init( Vector2D( flHalfWide, tall ), uv12  );

		vgui::surface()->DrawTexturedPolygon( 4, verts );

		// draw the third quadrant
		uv11.Init( 0.0f, 0.5f );
		uv21.Init( 0.5f, 0.5f );
		uv22.Init( 0.5f, 1.0f );
		uv12.Init( 0.0f, 1.0f );

		verts[0].Init( Vector2D( 0.0f, flHalfTall ), uv11 );
		verts[1].Init( Vector2D( flHalfWide, flHalfTall ), uv21 );
		verts[2].Init( Vector2D( flHalfWide, tall ), uv22 );
		verts[3].Init( Vector2D( 0.0f, tall ), uv12  );

		vgui::surface()->DrawTexturedPolygon( 4, verts );

		// draw the partial fourth quadrant
		if ( flEndAngle > fl90degrees * 3.5f ) // > 315 degrees
		{
			uv11.Init( 0.0f, 0.0f );
			uv21.Init( 0.5f - ( tan(fl90degrees * 4.0f - flEndAngle) * 0.5 ), 0.0f );
			uv22.Init( 0.5f, 0.5f );
			uv12.Init( 0.0f, 0.5f );

			verts[0].Init( Vector2D( 0.0f, 0.0f ), uv11 );
			verts[1].Init( Vector2D( flHalfWide - ( tan(fl90degrees * 4.0f - flEndAngle) * flHalfTall ), 0.0f ), uv21 );
			verts[2].Init( Vector2D( flHalfWide, flHalfTall ), uv22 );
			verts[3].Init( Vector2D( 0.0f, flHalfTall ), uv12 );

			vgui::surface()->DrawTexturedPolygon( 4, verts );
		}
		else // <= 315 degrees
		{
			uv11.Init( 0.0f, 0.5f );
			uv21.Init( 0.0f, 0.5f - ( tan(flEndAngle - fl90degrees * 3.0f) * 0.5 ) );
			uv22.Init( 0.5f, 0.5f );
			uv12.Init( 0.0f, 0.5f );

			verts[0].Init( Vector2D( 0.0f, flHalfTall ), uv11 );
			verts[1].Init( Vector2D( 0.0f, flHalfTall - ( tan(flEndAngle - fl90degrees * 3.0f) * flHalfWide ) ), uv21 );
			verts[2].Init( Vector2D( flHalfWide, flHalfTall ), uv22 );
			verts[3].Init( Vector2D( 0.0f, flHalfTall ), uv12  );

			vgui::surface()->DrawTexturedPolygon( 4, verts );
		}
	}
	else if ( flEndAngle >= fl90degrees * 2.0f ) // >= 180 degrees
	{
		// draw the first and second quadrants
		uv11.Init( 0.5f, 0.0f );
		uv21.Init( 1.0f, 0.0f );
		uv22.Init( 1.0f, 1.0f );
		uv12.Init( 0.5, 1.0f );

		verts[0].Init( Vector2D( flHalfWide, 0.0f ), uv11 );
		verts[1].Init( Vector2D( wide, 0.0f ), uv21 );
		verts[2].Init( Vector2D( wide, tall ), uv22 );
		verts[3].Init( Vector2D( flHalfWide, tall ), uv12  );

		vgui::surface()->DrawTexturedPolygon( 4, verts );

		// draw the partial third quadrant
		if ( flEndAngle > fl90degrees * 2.5f ) // > 225 degrees
		{
			uv11.Init( 0.5f, 0.5f );
			uv21.Init( 0.5f, 1.0f );
			uv22.Init( 0.0f, 1.0f );
			uv12.Init( 0.0f, 0.5f + ( tan(fl90degrees * 3.0f - flEndAngle) * 0.5 ) );

			verts[0].Init( Vector2D( flHalfWide, flHalfTall ), uv11 );
			verts[1].Init( Vector2D( flHalfWide, tall ), uv21 );
			verts[2].Init( Vector2D( 0.0f, tall ), uv22 );
			verts[3].Init( Vector2D( 0.0f, flHalfTall + ( tan(fl90degrees * 3.0f - flEndAngle) * flHalfWide ) ), uv12 );

			vgui::surface()->DrawTexturedPolygon( 4, verts );
		}
		else // <= 225 degrees
		{
			uv11.Init( 0.5f, 0.5f );
			uv21.Init( 0.5f, 1.0f );
			uv22.Init( 0.5f - ( tan( flEndAngle - fl90degrees * 2.0f) * 0.5 ), 1.0f );
			uv12.Init( 0.5f, 0.5f );

			verts[0].Init( Vector2D( flHalfWide, flHalfTall ), uv11 );
			verts[1].Init( Vector2D( flHalfWide, tall ), uv21 );
			verts[2].Init( Vector2D( flHalfWide - ( tan(flEndAngle - fl90degrees * 2.0f) * flHalfTall ), tall ), uv22 );
			verts[3].Init( Vector2D( flHalfWide, flHalfTall ), uv12  );

			vgui::surface()->DrawTexturedPolygon( 4, verts );
		}
	}
	else if ( flEndAngle >= fl90degrees ) // >= 90 degrees
	{
		// draw the first quadrant
		uv11.Init( 0.5f, 0.0f );
		uv21.Init( 1.0f, 0.0f );
		uv22.Init( 1.0f, 0.5f );
		uv12.Init( 0.5f, 0.5f );

		verts[0].Init( Vector2D( flHalfWide, 0.0f ), uv11 );
		verts[1].Init( Vector2D( wide, 0.0f ), uv21 );
		verts[2].Init( Vector2D( wide, flHalfTall ), uv22 );
		verts[3].Init( Vector2D( flHalfWide, flHalfTall ), uv12  );

		vgui::surface()->DrawTexturedPolygon( 4, verts );

		// draw the partial second quadrant
		if ( flEndAngle > fl90degrees * 1.5f ) // > 135 degrees
		{
			uv11.Init( 0.5f, 0.5f );
			uv21.Init( 1.0f, 0.5f );
			uv22.Init( 1.0f, 1.0f );
			uv12.Init( 0.5f + ( tan(fl90degrees * 2.0f - flEndAngle) * 0.5f ), 1.0f );

			verts[0].Init( Vector2D( flHalfWide, flHalfTall ), uv11 );
			verts[1].Init( Vector2D( wide, flHalfTall ), uv21 );
			verts[2].Init( Vector2D( wide, tall ), uv22 );
			verts[3].Init( Vector2D( flHalfWide + ( tan(fl90degrees * 2.0f - flEndAngle) * flHalfTall ), tall ), uv12  );

			vgui::surface()->DrawTexturedPolygon( 4, verts );
		}
		else // <= 135 degrees
		{
			uv11.Init( 0.5f, 0.5f );
			uv21.Init( 1.0f, 0.5f );
			uv22.Init( 1.0f, 0.5f + ( tan(flEndAngle - fl90degrees) * 0.5f ) );
			uv12.Init( 0.5f, 0.5f );

			verts[0].Init( Vector2D( flHalfWide, flHalfTall ), uv11 );
			verts[1].Init( Vector2D( wide, flHalfTall ), uv21 );
			verts[2].Init( Vector2D( wide, flHalfTall + ( tan(flEndAngle - fl90degrees) * flHalfWide ) ), uv22 );
			verts[3].Init( Vector2D( flHalfWide, flHalfTall ), uv12  );

			vgui::surface()->DrawTexturedPolygon( 4, verts );
		}
	}
	else // > 0 degrees
	{
		if ( flEndAngle > fl90degrees / 2.0f ) // > 45 degrees
		{
			uv11.Init( 0.5f, 0.0f );
			uv21.Init( 1.0f, 0.0f );
			uv22.Init( 1.0f, 0.5f - ( tan(fl90degrees - flEndAngle) * 0.5 ) );
			uv12.Init( 0.5f, 0.5f );

			verts[0].Init( Vector2D( flHalfWide, 0.0f ), uv11 );
			verts[1].Init( Vector2D( wide, 0.0f ), uv21 );
			verts[2].Init( Vector2D( wide, flHalfTall - ( tan(fl90degrees - flEndAngle) * flHalfWide ) ), uv22 );
			verts[3].Init( Vector2D( flHalfWide, flHalfTall ), uv12  );

			vgui::surface()->DrawTexturedPolygon( 4, verts );
		}
		else // <= 45 degrees
		{
			uv11.Init( 0.5f, 0.0f );
			uv21.Init( 0.5 + ( tan(flEndAngle) * 0.5 ), 0.0f );
			uv22.Init( 0.5f, 0.5f );
			uv12.Init( 0.5f, 0.0f );

			verts[0].Init( Vector2D( flHalfWide, 0.0f ), uv11 );
			verts[1].Init( Vector2D( flHalfWide + ( tan(flEndAngle) * flHalfTall ), 0.0f ), uv21 );
			verts[2].Init( Vector2D( flHalfWide, flHalfTall ), uv22 );
			verts[3].Init( Vector2D( flHalfWide, 0.0f ), uv12  );

			vgui::surface()->DrawTexturedPolygon( 4, verts );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudTimeStatus::CTFHudTimeStatus( Panel *parent, const char *name ) : EditablePanel( parent, name )
{
	m_iTeamIndex = -1;

	m_pTimeValue = new CTFLabel( this, "TimePanelValue", "" );
	m_pProgressBar = NULL;
	m_pOvertimeLabel = NULL;
	m_pOvertimeBG = NULL;
	m_pSuddenDeathLabel = NULL;
	m_pSuddenDeathBG = NULL;
	m_pWaitingForPlayersBG = NULL;
	m_pWaitingForPlayersLabel = NULL;
	m_pSetupLabel = NULL;
	m_pSetupBG = NULL;
	m_pTimePanelBG = NULL;

	m_flNextThink = 0.0f;
	m_iTimerIndex = 0;

	m_iTimerDeltaHead = 0;
	for( int i = 0 ; i < NUM_TIMER_DELTA_ITEMS ; i++ )
	{
		m_TimerDeltaItems[i].m_flDieTime = 0.0f;
	}

	ListenForGameEvent( "teamplay_update_timer" );
	ListenForGameEvent( "teamplay_timer_time_added" );
	ListenForGameEvent( "localplayer_changeteam" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudTimeStatus::FireGameEvent( IGameEvent *event )
{
	const char *eventName = event->GetName();

	if ( !Q_strcmp( eventName, "teamplay_update_timer" ) )
	{
		SetExtraTimePanels();
	}
	else if ( !Q_strcmp( eventName, "teamplay_timer_time_added" ) )
	{
		int iIndex = event->GetInt( "timer", -1 );
		int nSeconds = event->GetInt( "seconds_added", 0 );

		SetTimeAdded( iIndex, nSeconds );
	}
	else if ( !Q_strcmp( eventName, "localplayer_changeteam" ) )
	{
		SetTeamBackground();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudTimeStatus::SetTimeAdded( int iIndex, int nSeconds )
{
	if (  m_iTimerIndex != iIndex ) // make sure this is the timer we're displaying in the HUD
		return;

	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( nSeconds != 0 && pPlayer )
	{
		// create a delta item that floats off the top
		timer_delta_t *pNewDeltaItem = &m_TimerDeltaItems[m_iTimerDeltaHead];

		m_iTimerDeltaHead++;
		m_iTimerDeltaHead %= NUM_TIMER_DELTA_ITEMS;

		pNewDeltaItem->m_flDieTime = gpGlobals->curtime + m_flDeltaLifetime;
		pNewDeltaItem->m_nAmount = nSeconds;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudTimeStatus::CheckClockLabelLength( CTFLabel *pLabel, CTFImagePanel *pBG )
{
	if ( !pLabel || ! pBG )
		return;

	int textWide, textTall;
	pLabel->GetContentSize( textWide, textTall );

	// make sure our string isn't longer than the label it's in
	if ( textWide > pLabel->GetWide() )
	{
		int xStart, yStart, wideStart, tallStart;
		pLabel->GetBounds( xStart, yStart, wideStart, tallStart );
		
		int newXPos = xStart + ( wideStart / 2.0f ) - ( textWide / 2.0f );
		pLabel->SetBounds(  newXPos, yStart, textWide, tallStart );
	}

	// turn off the background if our text label is wider than it is
	if ( pLabel->GetWide() > pBG->GetWide() )
	{
		pBG->SetVisible( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudTimeStatus::SetExtraTimePanels()
{
	if ( !TFGameRules() )
		return;

	if ( m_pSetupBG && m_pSetupLabel )
	{
		CTeamRoundTimer *pTimer = dynamic_cast< CTeamRoundTimer* >( ClientEntityList().GetEnt( m_iTimerIndex ) );
		// get the time remaining (in seconds)
		if ( pTimer )
		{
			bool bInSetup = TFGameRules()->InSetup();
			m_pSetupBG->SetVisible( bInSetup );
			m_pSetupLabel->SetVisible( bInSetup );
		}
	}

	CTeamRoundTimer *pTimer = dynamic_cast< CTeamRoundTimer* >( ClientEntityList().GetEnt( m_iTimerIndex ) );
	if ( !pTimer )
		return;

	// Set the Sudden Death panels to be visible
	if ( m_pSuddenDeathBG && m_pSuddenDeathLabel )
	{
		bool bInSD = TFGameRules()->InStalemate();
		m_pSuddenDeathBG->SetVisible( bInSD );
		m_pSuddenDeathLabel->SetVisible( bInSD );
	}

	if ( m_pOvertimeBG && m_pOvertimeLabel )
	{
		bool bInOver = TFGameRules()->InOvertime();

		// KOTH timers go into overtime when they run out
		if ( TFGameRules()->IsInKothMode() )
		{
			bInOver = ( pTimer->GetTimeRemaining() <= 0 );
		}

		if ( bInOver )
		{
			if ( !m_pOvertimeBG->IsVisible() )
			{
				m_pOvertimeLabel->SetAlpha( 0 );
				m_pOvertimeBG->SetAlpha( 0 );

				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "OvertimeShow" ); 

				// need to turn off the SuddenDeath images if they're on
				m_pSuddenDeathBG->SetVisible( false );
				m_pSuddenDeathLabel->SetVisible( false );
			}

			m_pOvertimeBG->SetVisible( true );
			m_pOvertimeLabel->SetVisible( true );

			CheckClockLabelLength( m_pOvertimeLabel, m_pOvertimeBG );
		}
		else
		{
			m_pOvertimeBG->SetVisible( false );
			m_pOvertimeLabel->SetVisible( false );
		}
	}

	if ( m_pWaitingForPlayersBG && m_pWaitingForPlayersLabel )
	{
		bool bInWaitingForPlayers = TFGameRules()->IsInWaitingForPlayers();
		m_pWaitingForPlayersBG->SetVisible( bInWaitingForPlayers );
		m_pWaitingForPlayersLabel->SetVisible( bInWaitingForPlayers );

		if ( bInWaitingForPlayers )
		{
			// can't be waiting for players *AND* in setup at the same time
			if ( m_pSetupBG && m_pSetupLabel )
			{
				m_pSetupBG->SetVisible( false );
				m_pSetupLabel->SetVisible( false );
			}

			CheckClockLabelLength( m_pWaitingForPlayersLabel, m_pWaitingForPlayersBG );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudTimeStatus::Reset()
{
	m_flNextThink = gpGlobals->curtime + 0.05f;
	m_iTimerIndex = 0;

	m_iTimerDeltaHead = 0;
	for( int i = 0 ; i < NUM_TIMER_DELTA_ITEMS ; i++ )
	{
		m_TimerDeltaItems[i].m_flDieTime = 0.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudTimeStatus::ApplySchemeSettings( IScheme *pScheme )
{
	// load control settings...
	LoadControlSettings( "resource/UI/HudObjectiveTimePanel.res" );

	m_pTimeValue = dynamic_cast<CTFLabel *>( FindChildByName( "TimePanelValue" ) );
	m_pProgressBar = dynamic_cast<CTFProgressBar *>( FindChildByName( "TimePanelProgressBar" ) );

	m_pOvertimeLabel = dynamic_cast<CTFLabel *>( FindChildByName( "OvertimeLabel" ) );
	m_pOvertimeBG = dynamic_cast<CTFImagePanel *>( FindChildByName( "OvertimeBG" ) );

	m_pSuddenDeathLabel = dynamic_cast<CTFLabel *>( FindChildByName( "SuddenDeathLabel" ) );
	m_pSuddenDeathBG = dynamic_cast<CTFImagePanel *>( FindChildByName( "SuddenDeathBG" ) );

	m_pWaitingForPlayersLabel = dynamic_cast<CTFLabel *>( FindChildByName( "WaitingForPlayersLabel" ) );
	m_pWaitingForPlayersBG = dynamic_cast<CTFImagePanel *>( FindChildByName("WaitingForPlayersBG" ) );

	m_pSetupLabel = dynamic_cast<CTFLabel *>( FindChildByName( "SetupLabel" ) );
	m_pSetupBG = dynamic_cast<CTFImagePanel *>( FindChildByName("SetupBG" ) );

	m_pTimePanelBG = dynamic_cast<CTFImagePanel *>( FindChildByName( "TimePanelBG" ) );

	m_flNextThink = 0.0f;
	m_iTimerIndex = 0;

	SetTeamBackground();
	SetExtraTimePanels();

	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudTimeStatus::OnThink()
{
	if ( m_flNextThink < gpGlobals->curtime )
	{
		CTeamRoundTimer *pTimer = dynamic_cast< CTeamRoundTimer* >( ClientEntityList().GetEnt( m_iTimerIndex ) );
		// get the time remaining (in seconds)
		if ( pTimer )
		{
			int nTotalTime = pTimer->GetTimerMaxLength();
			int nTimeRemaining = pTimer->GetTimeRemaining();

			if ( m_pTimeValue && m_pTimeValue->IsVisible() )
			{
				// set our label
				int nMinutes = 0;
				int nSeconds = 0;
				char temp[256];

				if ( nTimeRemaining <= 0 )
				{
					nMinutes = 0;
					nSeconds = 0;
				}
				else
				{
					nMinutes = nTimeRemaining / 60;
					nSeconds = nTimeRemaining % 60;
				}				

				Q_snprintf( temp, sizeof( temp ), "%d:%02d", nMinutes, nSeconds );
				m_pTimeValue->SetText( temp );
			}
	
			// let the progress bar know the percentage of time that's passed ( 0.0 -> 1.0 )
			if ( m_pProgressBar && m_pProgressBar->IsVisible() )
			{
				m_pProgressBar->SetPercentage( ( (float)nTotalTime - nTimeRemaining ) / (float)nTotalTime );
			}
		}

		m_flNextThink = gpGlobals->curtime + 0.1f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Paint the deltas
//-----------------------------------------------------------------------------
void CTFHudTimeStatus::Paint( void )
{
	BaseClass::Paint();

	for ( int i = 0 ;  i < NUM_TIMER_DELTA_ITEMS ; i++ )
	{
		// update all the valid delta items
		if ( m_TimerDeltaItems[i].m_flDieTime > gpGlobals->curtime )
		{
			// position and alpha are determined from the lifetime
			// color is determined by the delta - green for positive, red for negative

			Color c = ( m_TimerDeltaItems[i].m_nAmount > 0 ) ? m_DeltaPositiveColor : m_DeltaNegativeColor;

			float flLifetimePercent = ( m_TimerDeltaItems[i].m_flDieTime - gpGlobals->curtime ) / m_flDeltaLifetime;

			// fade out after half our lifetime
			if ( flLifetimePercent < 0.5 )
			{
				c[3] = (int)( 255.0f * ( flLifetimePercent / 0.5 ) );
			}

			float flHeight = ( m_flDeltaItemStartPos - m_flDeltaItemEndPos );
			float flYPos = m_flDeltaItemEndPos + flLifetimePercent * flHeight;

			vgui::surface()->DrawSetTextFont( m_hDeltaItemFont );
			vgui::surface()->DrawSetTextColor( c );
			vgui::surface()->DrawSetTextPos( m_flDeltaItemX, (int)flYPos );

			wchar_t wBuf[20];
			int nMinutes, nSeconds;
			int nClockTime = ( m_TimerDeltaItems[i].m_nAmount > 0 ) ? m_TimerDeltaItems[i].m_nAmount : ( m_TimerDeltaItems[i].m_nAmount * -1 );
			nMinutes = nClockTime / 60;
			nSeconds = nClockTime % 60;

			if ( m_TimerDeltaItems[i].m_nAmount > 0 )
			{
				_snwprintf( wBuf, sizeof(wBuf)/sizeof(wchar_t), L"+%d:%02d", nMinutes, nSeconds );
			}
			else
			{
				_snwprintf( wBuf, sizeof(wBuf)/sizeof(wchar_t), L"-%d:%02d", nMinutes, nSeconds );
			}

			vgui::surface()->DrawPrintText( wBuf, wcslen(wBuf), FONT_DRAW_NONADDITIVE );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Use the team's timer background (our own team unless this timer belongs to one)
//-----------------------------------------------------------------------------
void CTFHudTimeStatus::SetTeamBackground( void )
{
	if ( TFGameRules() && m_pTimePanelBG )
	{
		int iTeamNumber = GetLocalPlayerTeam();
		if ( m_iTeamIndex > TEAM_INVALID )
		{
			iTeamNumber = m_iTeamIndex;
		}

		const char *pszImage;
		switch ( iTeamNumber )
		{
		case TF_TEAM_RED:
			pszImage = "../hud/objectives_timepanel_red_bg";
			break;
		case TF_TEAM_BLUE:
			pszImage = "../hud/objectives_timepanel_blue_bg";
			break;
		case FO_TEAM_GREEN:
			pszImage = "../hud/objectives_timepanel_green_bg";
			break;
		case FO_TEAM_YELLOW:
			pszImage = "../hud/objectives_timepanel_yellow_bg";
			break;
		case FO_TEAM_PURPLE:
			pszImage = "../hud/objectives_timepanel_purple_bg";
			break;
		case FO_TEAM_PINK:
			pszImage = "../hud/objectives_timepanel_pink_bg";
			break;
		default:
			pszImage = "../hud/objectives_timepanel_black_bg";
			break;
		}

		m_pTimePanelBG->SetImage( pszImage );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudTimeStatus::SetTimerIndex( int index )
{
	m_iTimerIndex = ( index >= 0 ) ? index : 0;
	SetExtraTimePanels();
}

DECLARE_HUDELEMENT( CTFHudKothTimeStatus );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudKothTimeStatus::CTFHudKothTimeStatus( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudKothTimeStatus" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_pActiveKothTimerPanel = NULL;

	m_pBlueKothTimer = new CTFHudTimeStatus( this, "BlueTimer" );
	m_pBlueKothTimer->SetTeam( TF_TEAM_BLUE );

	m_pRedKothTimer = new CTFHudTimeStatus( this, "RedTimer" );
	m_pRedKothTimer->SetTeam( TF_TEAM_RED );

	m_pGreenKothTimer = new CTFHudTimeStatus( this, "GreenTimer" );
	m_pGreenKothTimer->SetTeam( FO_TEAM_GREEN );

	m_pYellowKothTimer = new CTFHudTimeStatus( this, "YellowTimer" );
	m_pYellowKothTimer->SetTeam( FO_TEAM_YELLOW );

	m_pPurpleKothTimer = new CTFHudTimeStatus( this, "PurpleTimer" );
	m_pPurpleKothTimer->SetTeam( FO_TEAM_PURPLE );

	m_pPinkKothTimer = new CTFHudTimeStatus( this, "PinkTimer" );
	m_pPinkKothTimer->SetTeam( FO_TEAM_PINK );

	m_pActiveTimerBG = new ImagePanel( this, "ActiveTimerBG" );

	RegisterForRenderGroup( "mid" );
	RegisterForRenderGroup( "commentary" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudKothTimeStatus::ApplySchemeSettings( IScheme *pScheme )
{
	if ( !TFGameRules() )
		return;

	KeyValues *pConditions = NULL;
	if ( TFGameRules()->m_nExtraTeamMode == 1 )
	{
		pConditions = new KeyValues( "conditions" );
		AddSubKeyNamed( pConditions, "if_threeteams" );
	}
	else if ( TFGameRules()->m_nExtraTeamMode == 2 )
	{
		pConditions = new KeyValues( "conditions" );
		AddSubKeyNamed( pConditions, "if_sixteams" );
	}

	// The 3/6 team layouts need room for more timers
	SetPos( (double)GetXPos() - ( ScreenWidth() / 640.0f ) * 73.0f, GetYPos() );
	SetWide( GetWide() * 2 );

	// load control settings...
	LoadControlSettings( "resource/UI/HudObjectiveKothTimePanel.res", NULL, NULL, pConditions );

	BaseClass::ApplySchemeSettings( pScheme );

	m_nOriginalActiveTimerBGYPos = m_pActiveTimerBG->GetYPos();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudKothTimeStatus::ShouldDraw( void )
{
	if ( !TFGameRules() )
		return false;

	if ( !TFGameRules()->IsInKothMode() )
		return false;

	if ( TFGameRules()->IsInWaitingForPlayers() )
		return false;

	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudKothTimeStatus::Reset( void )
{
	if ( m_pBlueKothTimer )
	{
		m_pBlueKothTimer->Reset();
	}

	if ( m_pRedKothTimer )
	{
		m_pRedKothTimer->Reset();
	}

	if ( m_pGreenKothTimer )
	{
		m_pGreenKothTimer->Reset();
	}

	if ( m_pYellowKothTimer )
	{
		m_pYellowKothTimer->Reset();
	}

	if ( m_pPurpleKothTimer )
	{
		m_pPurpleKothTimer->Reset();
	}

	if ( m_pPinkKothTimer )
	{
		m_pPinkKothTimer->Reset();
	}

	UpdateActiveTeam();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudKothTimeStatus::Think( void )
{
	if ( !TFGameRules() )
		return;

	if ( !m_pBlueKothTimer || !m_pRedKothTimer )
		return;

	// Hide the timers in freezecam
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( pPlayer && pPlayer->GetObserverMode() == OBS_MODE_FREEZECAM )
	{
		m_pBlueKothTimer->SetVisible( false );
		m_pRedKothTimer->SetVisible( false );
		m_pGreenKothTimer->SetVisible( false );
		m_pYellowKothTimer->SetVisible( false );
		m_pPurpleKothTimer->SetVisible( false );
		m_pPinkKothTimer->SetVisible( false );
		m_pActiveTimerBG->SetVisible( false );
		return;
	}

	C_TeamRoundTimer *pTimer = dynamic_cast< C_TeamRoundTimer* >( ClientEntityList().GetEnt( m_pBlueKothTimer->GetTimerIndex() ) );

	CTFHudTimeStatus *pActiveTimer = NULL;
	if ( !pTimer )
	{
		pTimer = TFGameRules()->GetBlueKothRoundTimer();
		if ( pTimer )
		{
			if ( m_pBlueKothTimer->GetTimerIndex() != pTimer->index )
			{
				m_pBlueKothTimer->SetTimerIndex( pTimer->index );
			}
		}
	}

	if ( pTimer && !pTimer->IsDormant() && !pTimer->IsDisabled() && !pTimer->IsTimerPaused() )
	{
		pActiveTimer = m_pBlueKothTimer;
	}

	pTimer = dynamic_cast< C_TeamRoundTimer* >( ClientEntityList().GetEnt( m_pRedKothTimer->GetTimerIndex() ) );
	if ( !pTimer )
	{
		pTimer = TFGameRules()->GetRedKothRoundTimer();
		if ( pTimer )
		{
			if ( m_pRedKothTimer->GetTimerIndex() != pTimer->index )
			{
				m_pRedKothTimer->SetTimerIndex( pTimer->index );
			}
		}
	}

	if ( pTimer && !pTimer->IsDormant() && !pTimer->IsDisabled() && !pTimer->IsTimerPaused() )
	{
		pActiveTimer = m_pRedKothTimer;
	}

	pTimer = dynamic_cast< C_TeamRoundTimer* >( ClientEntityList().GetEnt( m_pGreenKothTimer->GetTimerIndex() ) );
	if ( !pTimer )
	{
		pTimer = TFGameRules()->GetGreenKothRoundTimer();
		if ( pTimer )
		{
			m_pGreenKothTimer->SetTimerIndex( pTimer->index );
		}
	}

	if ( pTimer && !pTimer->IsDormant() && !pTimer->IsDisabled() && !pTimer->IsTimerPaused() )
	{
		pActiveTimer = m_pGreenKothTimer;
	}

	pTimer = dynamic_cast< C_TeamRoundTimer* >( ClientEntityList().GetEnt( m_pYellowKothTimer->GetTimerIndex() ) );
	if ( !pTimer )
	{
		pTimer = TFGameRules()->GetYellowKothRoundTimer();
		if ( pTimer )
		{
			m_pYellowKothTimer->SetTimerIndex( pTimer->index );
		}
	}

	if ( pTimer && !pTimer->IsDormant() && !pTimer->IsDisabled() && !pTimer->IsTimerPaused() )
	{
		pActiveTimer = m_pYellowKothTimer;
	}

	pTimer = dynamic_cast< C_TeamRoundTimer* >( ClientEntityList().GetEnt( m_pPurpleKothTimer->GetTimerIndex() ) );
	if ( !pTimer )
	{
		pTimer = TFGameRules()->GetPurpleKothRoundTimer();
		if ( pTimer )
		{
			m_pPurpleKothTimer->SetTimerIndex( pTimer->index );
		}
	}

	if ( pTimer && !pTimer->IsDormant() && !pTimer->IsDisabled() && !pTimer->IsTimerPaused() )
	{
		pActiveTimer = m_pPurpleKothTimer;
	}

	pTimer = dynamic_cast< C_TeamRoundTimer* >( ClientEntityList().GetEnt( m_pPinkKothTimer->GetTimerIndex() ) );
	if ( !pTimer )
	{
		pTimer = TFGameRules()->GetPinkKothRoundTimer();
		if ( pTimer )
		{
			m_pPinkKothTimer->SetTimerIndex( pTimer->index );
		}
	}

	if ( pTimer && !pTimer->IsDormant() && !pTimer->IsDisabled() && !pTimer->IsTimerPaused() )
	{
		pActiveTimer = m_pPinkKothTimer;
	}

	if ( !m_pBlueKothTimer->IsVisible() || !m_pRedKothTimer->IsVisible() )
	{
		m_pBlueKothTimer->SetVisible( true );
		m_pRedKothTimer->SetVisible( true );

		// If our spectator GUI is visible, invalidate its layout so that it moves the reinforcement label
		if ( g_pSpectatorGUI )
		{
			g_pSpectatorGUI->InvalidateLayout();
		}
	}

	if ( TFGameRules()->m_nExtraTeamMode > 0 )
	{
		if ( !m_pGreenKothTimer->IsVisible() )
		{
			m_pGreenKothTimer->SetVisible( true );

			if ( g_pSpectatorGUI )
			{
				g_pSpectatorGUI->InvalidateLayout();
			}
		}

		if ( TFGameRules()->m_nExtraTeamMode == 2 && ( !m_pYellowKothTimer->IsVisible() || !m_pPurpleKothTimer->IsVisible() || !m_pPinkKothTimer->IsVisible() ) )
		{
			m_pYellowKothTimer->SetVisible( true );
			m_pPurpleKothTimer->SetVisible( true );
			m_pPinkKothTimer->SetVisible( true );

			if ( g_pSpectatorGUI )
			{
				g_pSpectatorGUI->InvalidateLayout();
			}
		}
	}
	else
	{
		m_pGreenKothTimer->SetVisible( false );
		m_pYellowKothTimer->SetVisible( false );
		m_pPurpleKothTimer->SetVisible( false );
		m_pPinkKothTimer->SetVisible( false );
	}

	if ( m_pActiveKothTimerPanel )
	{
		m_pActiveKothTimerPanel->SetExtraTimePanels();
	}

	if ( pActiveTimer != m_pActiveKothTimerPanel )
	{
		m_pActiveKothTimerPanel = pActiveTimer;
		UpdateActiveTeam();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudKothTimeStatus::UpdateActiveTeam( void )
{
	if ( !m_pActiveTimerBG )
		return;

	if ( m_pActiveKothTimerPanel )
	{
		m_pActiveTimerBG->SetVisible( true );

		int iTeam = m_pActiveKothTimerPanel->GetTeam();
		if ( iTeam == TF_TEAM_BLUE )
		{
			if ( TFGameRules() && TFGameRules()->m_nExtraTeamMode == 1 )
				m_pActiveTimerBG->SetPos( m_n3BlueActiveXPos, m_nOriginalActiveTimerBGYPos );
			else if ( TFGameRules() && TFGameRules()->m_nExtraTeamMode == 2 )
				m_pActiveTimerBG->SetPos( m_n6BlueActiveXPos, m_nOriginalActiveTimerBGYPos );
			else
				m_pActiveTimerBG->SetPos( m_nBlueActiveXPos, m_nOriginalActiveTimerBGYPos );
		}
		else if ( iTeam == TF_TEAM_RED )
		{
			if ( TFGameRules() && TFGameRules()->m_nExtraTeamMode == 1 )
				m_pActiveTimerBG->SetPos( m_n3RedActiveXPos, m_nOriginalActiveTimerBGYPos );
			else if ( TFGameRules() && TFGameRules()->m_nExtraTeamMode == 2 )
				m_pActiveTimerBG->SetPos( m_n6RedActiveXPos, m_nOriginalActiveTimerBGYPos );
			else
				m_pActiveTimerBG->SetPos( m_nRedActiveXPos, m_nOriginalActiveTimerBGYPos );
		}
		else if ( iTeam == FO_TEAM_GREEN )
		{
			if ( TFGameRules() && TFGameRules()->m_nExtraTeamMode == 2 )
				m_pActiveTimerBG->SetPos( m_n6GreenActiveXPos, m_nOriginalActiveTimerBGYPos );
			else
				m_pActiveTimerBG->SetPos( m_nGreenActiveXPos, m_nOriginalActiveTimerBGYPos );
		}
		else if ( iTeam == FO_TEAM_YELLOW )
		{
			m_pActiveTimerBG->SetPos( m_nYellowActiveXPos, m_nOriginalActiveTimerBGYPos );
		}
		else if ( iTeam == FO_TEAM_PURPLE )
		{
			m_pActiveTimerBG->SetPos( m_nPurpleActiveXPos, m_nOriginalActiveTimerBGYPos );
		}
		else if ( iTeam == FO_TEAM_PINK )
		{
			m_pActiveTimerBG->SetPos( m_nPinkActiveXPos, m_nOriginalActiveTimerBGYPos );
		}

		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( this, "ActiveTimerBGPulse" );
	}
	else
	{
		m_pActiveTimerBG->SetVisible( false );
	}
}

DECLARE_HUDELEMENT( CTFHudObjectiveStatus );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudObjectiveStatus::CTFHudObjectiveStatus( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudObjectiveStatus" ) 
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_pFlagPanel = new CTFHudFlagObjectives( this, "ObjectiveStatusFlagPanel" );
	m_p3FlagPanel = new CFOHud3FlagObjectives(this, "ObjectiveStatus3FlagPanel");
	m_p6FlagPanel = new CFOHud6FlagObjectives(this, "ObjectiveStatus6FlagPanel");
	m_pGeneratorPanel = new CFOHudGeneratorObjectives(this, "ObjectiveStatusGeneratorPanel");
	m_p3GeneratorPanel = new CFOHud3GeneratorObjectives(this, "ObjectiveStatus3GeneratorPanel");
	m_p6GeneratorPanel = new CFOHud6GeneratorObjectives(this, "ObjectiveStatus6GeneratorPanel");
	m_pTimePanel = new CTFHudTimeStatus( this, "ObjectiveStatusTimePanel" );
	m_pKothTimePanel = new CTFHudKothTimeStatus( "HudKothTimeStatus" );
	m_pEscortPanel = new CTFHudEscort(this, "ObjectiveStatusEscort", TF_TEAM_BLUE, false);
	m_pEscortRacePanel = new CTFHudMultipleEscort(this, "ObjectiveStatusMultipleEscort");
	m_pControlPointIconsPanel = NULL;
	m_pControlPointProgressBar = new CControlPointProgressBar( this );

	SetHiddenBits( 0 );

	RegisterForRenderGroup( "mid" );
	RegisterForRenderGroup( "commentary" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudObjectiveStatus::ApplySchemeSettings( IScheme *pScheme )
{
	if ( !m_pControlPointIconsPanel )
	{
		m_pControlPointIconsPanel = GET_HUDELEMENT( CHudControlPointIcons );
		m_pControlPointIconsPanel->SetParent( this );
	}

	if ( m_pControlPointProgressBar )
	{
		m_pControlPointProgressBar->InvalidateLayout( true, true );
	}

	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudObjectiveStatus::Reset()
{
	if ( m_pTimePanel )
	{
		m_pTimePanel->Reset();
	}

	if ( m_pFlagPanel )
	{
		m_pFlagPanel->Reset();
	}

	if (m_p3FlagPanel)
	{
		m_p3FlagPanel->Reset();
	}

	if (m_p6FlagPanel)
	{
		m_p6FlagPanel->Reset();
	}

	if (m_pEscortPanel)
	{
		m_pEscortPanel->Reset();
	}

	if (m_pEscortRacePanel)
	{
		m_pEscortRacePanel->Reset();
	}

	if ( m_pKothTimePanel )
	{
		m_pKothTimePanel->Reset();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CControlPointProgressBar *CTFHudObjectiveStatus::GetControlPointProgressBar( void )
{
	return m_pControlPointProgressBar;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudObjectiveStatus::SetVisiblePanels( void )
{
	if ( !TFGameRules() )
		return;

	// turn off the flag panel
	if (m_pFlagPanel && m_pFlagPanel->IsVisible())
	{
		m_pFlagPanel->SetVisible(false);
	}

	// turn off the 3flag panel
	if (m_p3FlagPanel && m_p3FlagPanel->IsVisible())
	{
		m_p3FlagPanel->SetVisible(false);
	}

	// turn off the 6flag panel
	if (m_p6FlagPanel && m_p6FlagPanel->IsVisible())
	{
		m_p6FlagPanel->SetVisible(false);
	}

	// turn off the control point icons
	if (m_pControlPointIconsPanel && m_pControlPointIconsPanel->IsVisible())
	{
		m_pControlPointIconsPanel->SetVisible(false);
	}

	// turn off the payload panel
	if (m_pEscortPanel && m_pEscortPanel->IsVisible())
	{
		m_pEscortPanel->SetVisible(false);
	}

	// turn off the payload race panel
	if (m_pEscortRacePanel && m_pEscortRacePanel->IsVisible())
	{
		m_pEscortRacePanel->SetVisible(false);
	}

	// turn off the generator panel
	if (m_pGeneratorPanel && m_pGeneratorPanel->IsVisible())
	{
		m_pGeneratorPanel->SetVisible(false);
	}

	// turn off the 3generator panel
	if (m_p3GeneratorPanel && m_p3GeneratorPanel->IsVisible())
	{
		m_p3GeneratorPanel->SetVisible(false);
	}

	// turn off the 6generator panel
	if (m_p6GeneratorPanel && m_p6GeneratorPanel->IsVisible())
	{
		m_p6GeneratorPanel->SetVisible(false);
	}

	// turn off the koth timers
	if ( m_pKothTimePanel && m_pKothTimePanel->IsVisible() )
	{
		m_pKothTimePanel->SetVisible( false );
	}


	// only draw the flag panel for CTF maps
	if ( TFGameRules()->GetGameType() == TF_GAMETYPE_CTF )
	{
		if (TFGameRules()->ExtraTeamMode() == 1)
		{
			// turn on the 3flag panel
			if (m_p3FlagPanel && !m_p3FlagPanel->IsVisible())
			{
				m_p3FlagPanel->SetVisible(true);
			}
		}
		else if (TFGameRules()->ExtraTeamMode() == 2)
		{
			// turn on the 6flag panel
			if (m_p6FlagPanel && !m_p6FlagPanel->IsVisible())
			{
				m_p6FlagPanel->SetVisible(true);
			}
		}
		else
		{
			// turn on the flag panel
			if (m_pFlagPanel && !m_pFlagPanel->IsVisible())
			{
				m_pFlagPanel->SetVisible(true);
			}
		}
	}
	else if ( TFGameRules()->GetGameType() == TF_GAMETYPE_CP || TFGameRules()->GetGameType() == TF_GAMETYPE_ARENA)
	{
		// turn on the koth timers
		if ( TFGameRules()->IsInKothMode() && m_pKothTimePanel && !m_pKothTimePanel->IsVisible() )
		{
			m_pKothTimePanel->SetVisible( true );
		}

		// turn on the control point icons
		if ( m_pControlPointIconsPanel && !m_pControlPointIconsPanel->IsVisible() )
		{
			m_pControlPointIconsPanel->SetVisible( true );
		}
	}
	else if (TFGameRules()->GetGameType() == TF_GAMETYPE_ESCORT)
	{
		if (TeamplayRoundBasedRules()->HasMultipleTrains())
		{
			// turn on the payload race panel
			if (m_pEscortRacePanel && !m_pEscortRacePanel->IsVisible())
			{
				m_pEscortRacePanel->SetVisible(true);
			}
		}
		else
		{
			// turn on the payload panel
			if (m_pEscortPanel && !m_pEscortPanel->IsVisible())
			{
				m_pEscortPanel->SetVisible(true);
			}
		}
	}
	else if (TFGameRules()->GetGameType() == FO_GAMETYPE_DOM)
	{
		// turn on the control point icons
		if (m_pControlPointIconsPanel && !m_pControlPointIconsPanel->IsVisible())
		{
			m_pControlPointIconsPanel->SetVisible(true);
		}
	}
	else if (TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
	{
		// turn on the control point icons
		if (m_pControlPointIconsPanel && !m_pControlPointIconsPanel->IsVisible())
		{
			m_pControlPointIconsPanel->SetVisible(true);
		}

		if (TFGameRules()->ExtraTeamMode() == 1)
		{
			// turn on the 3flag panel
			if (m_p3FlagPanel && !m_p3FlagPanel->IsVisible())
			{
				m_p3FlagPanel->SetVisible(true);
			}
		}
		else if (TFGameRules()->ExtraTeamMode() == 2)
		{
			// turn on the 6flag panel
			if (m_p6FlagPanel && !m_p6FlagPanel->IsVisible())
			{
				m_p6FlagPanel->SetVisible(true);
			}
		}
		else
		{
			// turn on the flag panel
			if (m_pFlagPanel && !m_pFlagPanel->IsVisible())
			{
				m_pFlagPanel->SetVisible(true);
			}
		}
	}
	else if (TFGameRules()->GetGameType() == FO_GAMETYPE_GD)
	{
		// turn on the control point icons
		if (m_pControlPointIconsPanel && !m_pControlPointIconsPanel->IsVisible())
		{
			m_pControlPointIconsPanel->SetVisible(true);
		}

		if (TFGameRules()->ExtraTeamMode() == 1)
		{
			// turn on the 3generator panel
			if (m_p3GeneratorPanel && !m_p3GeneratorPanel->IsVisible())
			{
				m_p3GeneratorPanel->SetVisible(true);
			}
		}
		else if (TFGameRules()->ExtraTeamMode() == 2)
		{
			// turn on the 6generator panel
			if (m_p6GeneratorPanel && !m_p6GeneratorPanel->IsVisible())
			{
				m_p6GeneratorPanel->SetVisible(true);
			}
		}
		else
		{
			// turn on the generator panel
			if (m_pGeneratorPanel && !m_pGeneratorPanel->IsVisible())
			{
				m_pGeneratorPanel->SetVisible(true);
			}
		}
	}
	else if (TFGameRules()->GetGameType() == FO_GAMETYPE_DITR)
	{
		// turn on the control point icons
		if (m_pControlPointIconsPanel && !m_pControlPointIconsPanel->IsVisible())
		{
			m_pControlPointIconsPanel->SetVisible(true);
		}

		if (TFGameRules()->ExtraTeamMode() == 1)
		{
			// turn on the 3flag panel
			if (m_p3FlagPanel && !m_p3FlagPanel->IsVisible())
			{
				m_p3FlagPanel->SetVisible(true);
			}
		}
		else if (TFGameRules()->ExtraTeamMode() == 2)
		{
			// turn on the 6flag panel
			if (m_p6FlagPanel && !m_p6FlagPanel->IsVisible())
			{
				m_p6FlagPanel->SetVisible(true);
			}
		}
		else
		{
			// turn on the flag panel
			if (m_pFlagPanel && !m_pFlagPanel->IsVisible())
			{
				m_pFlagPanel->SetVisible(true);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudObjectiveStatus::Think()
{
	if ( !TeamplayRoundBasedRules() )
		return;

	SetVisiblePanels();

	// check for an active timer and turn the time panel on or off if we need to
	if ( m_pTimePanel )
	{
		// Don't draw in freezecam, or when the game's not running
		C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
		bool bDisplayTimer = !( pPlayer && pPlayer->GetObserverMode() == OBS_MODE_FREEZECAM );
		if ( bDisplayTimer )
		{
			// is the time panel still pointing at an active timer?
			int iCurrentTimer = m_pTimePanel->GetTimerIndex();
			CTeamRoundTimer *pTimer = dynamic_cast< CTeamRoundTimer* >( ClientEntityList().GetEnt( iCurrentTimer ) );

			if ( pTimer && !pTimer->IsDormant() && !pTimer->IsDisabled() && pTimer->ShowInHud() )
			{
				// the current timer is fine, make sure the panel is visible
				bDisplayTimer = true;
			}
			else if ( ObjectiveResource() )
			{
				// check for a different timer
				int iActiveTimer = ObjectiveResource()->GetTimerToShowInHUD();

				pTimer = dynamic_cast< CTeamRoundTimer* >( ClientEntityList().GetEnt( iActiveTimer ) );
				bDisplayTimer = ( iActiveTimer != 0 && pTimer && !pTimer->IsDormant() );
				m_pTimePanel->SetTimerIndex( iActiveTimer );
			}
		}

		if ( bDisplayTimer )
		{
			if ( !m_pTimePanel->IsVisible() )
			{
				m_pTimePanel->SetVisible( true );

				// If our spectator GUI is visible, invalidate its layout so that it moves the reinforcement label
				if ( g_pSpectatorGUI )
				{
					g_pSpectatorGUI->InvalidateLayout();
				}
			}
		}
		else if ( m_pTimePanel->IsVisible() )
		{
			m_pTimePanel->SetVisible( false );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudObjectiveStatus::TimerIsVisible( void )
{
	if ( m_pTimePanel )
		return m_pTimePanel->IsVisible();
	return false;
}
