//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "c_tf_player.h"
#include "iclientmode.h"
#include "ienginevgui.h"
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ProgressBar.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHudTeleconTeleportMeter : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudTeleconTeleportMeter, EditablePanel );

public:
	CHudTeleconTeleportMeter( const char *pElementName );

	virtual void	ApplySchemeSettings( IScheme *scheme );
	virtual bool	ShouldDraw( void );
	virtual void	OnTick( void );

private:
	vgui::ContinuousProgressBar *m_pTeleportMeter;
};

DECLARE_HUDELEMENT( CHudTeleconTeleportMeter );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudTeleconTeleportMeter::CHudTeleconTeleportMeter( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudTeleportMeter" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_pTeleportMeter = new ContinuousProgressBar( this, "TeleportMeter" );

	SetHiddenBits( HIDEHUD_MISCSTATUS );

	vgui::ivgui()->AddTickSignal( GetVPanel() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudTeleconTeleportMeter::ApplySchemeSettings( IScheme *pScheme )
{
	// load control settings...
	LoadControlSettings( "resource/UI/HudTeleportMeter.res" );

	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudTeleconTeleportMeter::ShouldDraw( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer || !pPlayer->IsPlayerClass( FO_CLASS_TELECON + 1) )
	{
		return false;
	}

	if ( !pPlayer->IsAlive() )
	{
		return false;
	}

	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudTeleconTeleportMeter::OnTick( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer )
		return;

	if ( m_pTeleportMeter )
	{
		m_pTeleportMeter->SetProgress( pPlayer->m_Shared.GetTeleconTeleportMeter() / 100.0f );
	}
}