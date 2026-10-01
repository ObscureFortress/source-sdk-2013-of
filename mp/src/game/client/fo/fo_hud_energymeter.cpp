//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: Saptrap energy meter
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
// Purpose: Shows the Saptrap's energy, which fills as enemies trigger its traps.
//-----------------------------------------------------------------------------
class CHudSaptrapEnergyMeter : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudSaptrapEnergyMeter, EditablePanel );

public:
	CHudSaptrapEnergyMeter( const char *pElementName );

	virtual void	ApplySchemeSettings( IScheme *scheme );
	virtual bool	ShouldDraw( void );
	virtual void	OnTick( void );

private:
	vgui::ContinuousProgressBar *m_pEnergyMeter;
};

DECLARE_HUDELEMENT( CHudSaptrapEnergyMeter );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudSaptrapEnergyMeter::CHudSaptrapEnergyMeter( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudEnergyMeter" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_pEnergyMeter = new ContinuousProgressBar( this, "EnergyMeter" );

	SetHiddenBits( HIDEHUD_MISCSTATUS );

	vgui::ivgui()->AddTickSignal( GetVPanel() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudSaptrapEnergyMeter::ApplySchemeSettings( IScheme *pScheme )
{
	// load control settings...
	LoadControlSettings( "resource/UI/HudEnergyMeter.res" );

	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudSaptrapEnergyMeter::ShouldDraw( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer || !pPlayer->IsPlayerClass( FO_CLASS_SAPTRAP + 1 ) )
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
void CHudSaptrapEnergyMeter::OnTick( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer )
		return;

	if ( m_pEnergyMeter )
	{
		m_pEnergyMeter->SetProgress( pPlayer->m_Shared.GetSaptrapEnergyMeter() / 100.0f );
	}
}
